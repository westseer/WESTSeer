# Worldwide Emerging Scientific Topic Seer

## 1. Motivation
Sciences are continuously evolving and fragmentising into increasingly more specialties. According to a study of the common patterns among the successful researchers, research success often starts with a wide exploration of the new specialties to optimise the research direction. However, many specialties are too new to be widely recognised because they are emerging topics at the time, each of which currently only has a small number of relevant papers even though the number is increasing fast. Therefore, manual exploration is challenging, labourous and time consuming. We design this computer program (i.e., worldwide emerging scientific topic seer) to automate the exploration and thus help researchers become more efficient.

## 2. Basic Idea
Since widely known specialties are usually already explored, the exploration should focus on the unrecognised specialties, i.e., the emerging topics. In this computing program, a user can specify a research scope using two different groups of keywords, each group consisting of similar keywords. The two groups are required because using only one group usually specifies too broad a scope, causing long and excessive data collection as well as endless data analysis. Then our program collects meta data of papers in the scope from OpenAlex, extract important terms and biterms in each paper, and analyse the emerging topic trends. More importantly, the program identifies the most important paper of each emerging topic, i.e., the one that initiated the basic idea of the topic. We called such a paper the influential innovative publication of the emerging topic. Knowing the influential innovative publication can help understand the emerging topic.

## 3. Comparison of Existing Computer Programs
There are some programs based on the RPYS method (e.g., CR Explorer), but RPYS is retrospective, i.e., only good for looking backward in time. The exploration should focusing on looking forward in time instead.

There is also CiteSpace, which is excellent for visualising the current hot spots of the research scope, but the hot spots are in the present, and aiming the research to a hot spot will typically leads to working on a mature specialty where the main research problems have already been well answered; CiteSeer also has a burst detection for detecting the increasingly hot papers, but the increasing hotness of a paper have many reasons, not always due to an emerging topic. The exploration should guarantee the results are emerging-topic relevent.

## 4. Outline of the Design
There are four main modules of the program: the data collector (OpenAlex.cpp and OpenAlex.h) module collects data from OpenAlex, the data storage module (ResearchScope.cpp and ResearchScope.h) maintains the database for the collected data and the intermediate results of the analysis, the topic trend analysis module (AETT.cpp and AETT.h) finds influential innovative publications and their emerging topics, the topic inspection module (ResearchTopic.cpp and ResearchTopic.h) analyses an emerging topic's current stat and predicts its trend.

## 5. Development Environment
CodeBlocks with TDM GCC x64 Toolchain. Unzip lib.zip.* before development. If dialogs are not shown in the Resources explorer, execute the following commands in the script console of CodeBlocks (under the View menu):

WxsRecoverWxsFile(_( "wxDialog wxs=\"wxsmith/LogDialog.wxs\" src=\"LogDialog.cpp\" hdr=\"LogDialog.h\" name=\"LogDialog\" language=\"CPP\" "));
WxsRecoverWxsFile(_( "wxDialog wxs=\"wxsmith/SettingsDialog.wxs\" src=\"SettingsDialog.cpp\" hdr=\"SettingsDialog.h\" name=\"SettingsDialog\" language=\"CPP\" "));
WxsRecoverWxsFile(_( "wxDialog wxs=\"wxsmith/OpenAlexImportDialog.wxs\" src=\"OpenAlexImportDialog.cpp\" hdr=\"OpenAlexImportDialog.h\" name=\"OpenAlexImportDialog\" language=\"CPP\" "));
WxsRecoverWxsFile(_( "wxDialog wxs=\"wxsmith/SQLDialog.wxs\" src=\"SQLDialog.cpp\" hdr=\"SQLDialog.h\" name=\"SQLDialog\" language=\"CPP\" "));
