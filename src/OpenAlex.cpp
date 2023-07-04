#include <OpenAlex.h>
#include <../WESTSeerApp.h>
#include <StringProcessing.h>
#include <wxFFileLog.h>
#include <GeneralConfig.h>
#include <sstream>
#include <regex>
#include <fstream>
#include <algorithm>
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <httplib.h>

void OpenAlex::init()
{
    GeneralConfig config;
    _y2 = WESTSeerApp::year();
    _y1 = _y2 - 5;
    _y0 = _y2 - 15;

    int numCombs = _scope.numCombinations();
    for (int y = _y2 - 1; y >= _y0; y--)
    {
        std::vector<std::string> urlsOfY;
        for (int i = 0; i < numCombs; i++)
        {
            std::string combination = _scope.getCombination(i);
            std::vector<std::string> kws = splitString(combination, "&");
            std::stringstream ss;
            ss << "/works?mailto=" << _email
               << "&per-page=200&filter=publication_year:"
               << y << ",language:en";
            ss << ",abstract.search:\""
               << kws[0] << "\"";
            ss << ",abstract.search:\""
               << kws[1] << "\"";
            urlsOfY.push_back(ss.str());
        }
        std::pair<int,std::vector<std::string>> yUrls(y, urlsOfY);
        _urls.push_back(yUrls);
    }
    _samples.resize(numCombs);
    _scope.init();
    _samplesOnly = false;
}

OpenAlex::OpenAlex(const std::string email, const std::string path, const std::string kws): _scope(path, kws)
{
    _email = email;
    init();
}

OpenAlex::~OpenAlex()
{
}

bool OpenAlex::finished()
{
    int numCombs = _scope.numCombinations();
    for (int y = _y2 - 1; y >= _y0; y--)
    {
        for (int i = 0; i < numCombs; i++)
        {
            if (!_scope.load(i, y))
                return false;
        }
    }
    return true;
}

const char *OpenAlex::name()
{
    return "Collect Data from OpenAlex";
}

int OpenAlex::numSteps()
{
    if (_urls.size() > 0)
        return _urls.size() * _urls[0].second.size();
    else
        return 0;
}

int getResultCount(const nlohmann::json& response)
{
    if (response.find("meta") == response.end())
    {
        return 0;
    }
    auto meta = response["meta"];
    if (meta.find("count") == meta.end())
    {
        return 0;
    }
    return meta["count"];
}

std::string getNextCursor(const nlohmann::json& response)
{
    if (response.find("meta") == response.end())
    {
        return "";
    }
    auto meta = response["meta"];
    if (meta.find("next_cursor") == meta.end())
    {
        return "";
    }
    auto nextCursor = meta["next_cursor"];
    if (nextCursor.is_null() || nextCursor == "")
    {
        return "";
    }
    return nextCursor;
}

void OpenAlex::doStep(int stepId)
{
    int numCombs = _scope.numCombinations();
    int i = stepId / numCombs;
    int j = stepId % numCombs;
    int y = _urls[i].first;
    if (_scope.load(j, y) || (i > 0 && _samplesOnly))
        return;

    std::vector<std::string> urlsOfY = _urls[i].second;
    std::string url = urlsOfY[j];
    std::map<uint64_t, Publication> pubsOfY;
    logDebug(url.c_str());

    // get first page of this download url
    httplib::Client client("https://api.openalex.org");
    auto res = client.Get(url + "&cursor=*");
    while (res->status != 200)
    {
        if (_cancelled.load() == true)
            return;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
            res = client.Get(url + "&cursor=*");
    }
    if (_cancelled.load() == true)
        return;

    // parse the response on the first page
    logDebug("parse the response on the first page");
    nlohmann::json response = nlohmann::json::parse(res->body);
    std::string nextCursor = getNextCursor(response);
    auto resultsOnPage = response["results"];
    int numResultsOnPage = resultsOnPage.size();
    _samples[j].clear();
    if (numResultsOnPage > 0)
    {
        for (auto &result: resultsOnPage)
        {
            Publication pub(result);
            pubsOfY[pub.id()] = pub;
            _samples[j].push_back(pub);
        }
    }

    if (_samplesOnly)
        return;

    // repeatedly get next page
    while (nextCursor != "")
    {
        // request next page
        logDebug("request next page");
        std::string pageURL = url + "&cursor=" + nextCursor;
        res = client.Get(pageURL);
        while (res->status != 200)
        {
            if (_cancelled.load() == true)
                return;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            res = client.Get(pageURL);
        }
        if (_cancelled.load() == true)
            return;

        // parse the page
        logDebug("parse the page");
        response = nlohmann::json::parse(res->body);
        nextCursor = getNextCursor(response);
        resultsOnPage = response["results"];
        for (auto &result: resultsOnPage)
        {
            Publication pub(result);
            pubsOfY[pub.id()] = pub;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    logDebug("save");
    _scope.save(j, y, pubsOfY);

    if (_cancelled.load() == true)
    {
        return;
    }

     // get references
     logDebug("get references");
     std::map<uint64_t, Publication> refsOfY;
     std::vector<uint64_t> newRefIds;
     if (_scope.getMissingRefIds(j, y, newRefIds))
     {
        // get new references
		for (size_t idxRef0 = 0; idxRef0 < newRefIds.size(); idxRef0 += 50)
        {
			size_t idxRef1 = idxRef0 + 50;
			if (idxRef1 > newRefIds.size())
			{
				idxRef1 = newRefIds.size();
			}

			// create url
			std::stringstream ssURL;
			ssURL << "/works?mailto=" << _email
				<< "&per-page=200&filter=language:en,ids.openalex:"
				<< Publication::convertId(newRefIds[idxRef0], 'W');
			for (size_t idxRef = idxRef0 + 1; idxRef < idxRef1; idxRef++)
            {
				ssURL << "|" << Publication::convertId(newRefIds[idxRef], 'W');
			}
			std::string url = ssURL.str();

			// make request
			res = client.Get(url);
			while (res->status != 200)
			{
				if (_cancelled.load() == true)
                {
					return;
				}
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
				res = client.Get(url);
			}
			if (_cancelled.load() == true)
            {
				return;
			}

			// parse response
			response = nlohmann::json::parse(res->body);
			auto resultsOfResponse = response["results"];
			for (auto result: resultsOfResponse) {
                Publication refPub(result);
				refsOfY[refPub.id()] = refPub;
			}

			if (_cancelled.load() == true)
            {
				return;
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
		_scope.save(refsOfY);
     }
     _scope.save(j, y);
}


