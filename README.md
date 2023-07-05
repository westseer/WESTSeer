# Worldwide Emerging Scientific Topic Seer

## 1. Motivation
Sciences are continuously evolving and fragmentising into increasingly more specialties. According to a study of the common patterns among the successful researchers, research success often starts with a wide exploration of the new specialties to optimise the research direction. However, many specialties are too new to be widely recognised because they are emerging topics at the time, each of which currently only has a small number of relevant papers even though the number is increasing fast. Therefore, manual exploration is challenging, labourous and time consuming. We design this computer program (i.e., worldwide emerging scientific topic seer) to automate the exploration and thus help researchers become more efficient.

## 2. Basic Idea
Since widely known specialties are usually already explored, the exploration should focus on the unrecognised specialties, i.e., the emerging topics. In this computing program, a user can specify a research scope using two different groups of keywords, each group consisting of similar keywords. The two groups are required because using only one group usually specifies too broad a scope, causing long and excessive data collection as well as endless data analysis. Then our program collects meta data of papers in the scope from OpenAlex, extract important terms and biterms in each paper, and analyse the emerging topic trends. More importantly, the program identifies the most important paper of each emerging topic, i.e., the one that initiated the basic idea of the topic. We called such a paper the influential innovative publication of the emerging topic. Knowing the influential innovative publication can help understand the emerging topic.

## 3. Quick Start
Tutorial link: [10 Minutes Tutorial](https://westseer.org/2023/07/05/westseer-tutorial-get-started-in-just-10-minutes/).

## 4. Test with Example Data (for Repreducibility Check)
The main algorithm of WESTSeer is AETT, which is a reproducible algorithm. In other words, given a set of input data, the algorithm always yields the same output results. To check AETT's reproducibility, please don't modify any parameters of WESTSeer except for user Email and database path. To get the backup database for reproducibility check, we should download the segmented zip files from [WESTSeer Example Data](https://github.com/westseer/westseer_example_data) (i.e., database.zip.001, database.zip.002, ..., database.zip.007), then unzip the database.sqlite file from them using 7zip. To import the backup database file into WESTSeer, select "File" -> "Options" from menu and then set "Path of Database" in "General" to the path of the database file. Then, in the main user interface of WESTSeer, select any research scope in the "Scope" choice box to start the data analysis of that scope.

When the analysis is finished, we should select "Test" mode in the "Mode" menu of WESTSeer. Then we can find the results shown in our paper. The results should be consistent with our paper, unless some options other than user Email and database path are changed.

## 5. Binary Releases for Windows.
Users who want binary releases should visit [WESTSeer Binary Releases for Windows](https://github.com/westseer/WESTSeer-Binary-Releases-for-Windows).

## 6. Outline of the Design
The pipeline of data analysis in this program is devided into tasks. For example, the task of data collection is implemented in OpenAlex.cpp and OpenAlex.h, the task of term extraction is implemented in TermExtraction.cpp and TermExtraction.h. All these tasks are written in a c++ class inherited from AbstractTask, which is a base class implemented in AbstractTask.cpp and AbstractTask.h. You can find these source files in src folder and include folder.

## 7. Development Environment
We use CodeBlocks with TDM GCC x64 Toolchain to develop this program. All dependencies are already contained in the repository. Just remember to unzip lib.zip.* before development.

If dialogs are not shown in the Resources explorer, execute the following commands in the script console of CodeBlocks (under the View menu):

WxsRecoverWxsFile(_( "wxDialog wxs=\"wxsmith/LogDialog.wxs\" src=\"LogDialog.cpp\" hdr=\"LogDialog.h\" name=\"LogDialog\" language=\"CPP\" "));
WxsRecoverWxsFile(_( "wxDialog wxs=\"wxsmith/SettingsDialog.wxs\" src=\"SettingsDialog.cpp\" hdr=\"SettingsDialog.h\" name=\"SettingsDialog\" language=\"CPP\" "));
WxsRecoverWxsFile(_( "wxDialog wxs=\"wxsmith/OpenAlexImportDialog.wxs\" src=\"OpenAlexImportDialog.cpp\" hdr=\"OpenAlexImportDialog.h\" name=\"OpenAlexImportDialog\" language=\"CPP\" "));
WxsRecoverWxsFile(_( "wxDialog wxs=\"wxsmith/SQLDialog.wxs\" src=\"SQLDialog.cpp\" hdr=\"SQLDialog.h\" name=\"SQLDialog\" language=\"CPP\" "));
