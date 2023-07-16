#ifndef RESEARCHSCOPE_H
#define RESEARCHSCOPE_H
#include <cstdint>
#include <string>
#include <vector>
#include <map>
#include <Publication.h>
class BitermWeight;
class TopicIdentification;

class ResearchScope
{
    public:
        ResearchScope(const std::string path, const std::string keywords);
        virtual ~ResearchScope();
        std::string getKeywords() const;
        bool init();
        int numCombinations() const;
        std::string getCombination(int i) const;
        int numPublications(const int y) const;
        bool load(const int y, std::map<uint64_t, Publication> &pubsOfY, std::map<uint64_t, Publication> &refsOfY);
        bool load(int idxComb, const int y, std::map<uint64_t, Publication> &pubOfY);
        bool load(int idxComb, const int y);
        bool save(int idxComb, const int y, const std::map<uint64_t, Publication> &pubsOfY);
        bool save(const std::map<uint64_t, Publication> &pubsOfY);
        bool save(int idxComb, const int y);
        bool removeOneYear(const int y);
        int getLatestYear(int yb, int ye);
        int getProgress(int yb, int ye);
        static bool remove(const std::string path, const std::string keywords);
        static bool remove(const std::string keywords);
        bool remove();
        bool getExistingRefIds(const int y, std::map<uint64_t, std::vector<uint64_t>> &refIdsOfId);
        bool getMissingRefIds(int idxComb, const int y, std::vector<uint64_t> &missingRefIds);
        Publication getPublication(uint64_t id);
        std::vector<Publication> getPublications(std::vector<uint64_t> ids);
        std::vector<Publication> getReferences(uint64_t id);
        std::vector<Publication> getCitations(uint64_t id, int ye);
        std::map<uint64_t,std::vector<Publication>> getCitations(const std::vector<uint64_t> &ids, int ye);
        std::vector<Publication> getTopicPublications(uint64_t id, int ye, BitermWeight *bw, TopicIdentification *ti);
        std::pair<std::string,std::string> getTopic(uint64_t id, int ye, TopicIdentification *ti);
        void writeWoS(int yb, int ye, std::string fileName);

        static std::vector<std::string> getResearchScopes(const std::string path);

    protected:
        bool storable();
        std::string getCombinations();

    private:
        std::string _path;
        std::vector<std::vector<std::string>> _kwss;
};

#endif // RESEARCHSCOPE_H
