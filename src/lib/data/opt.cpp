#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "opt.h"

Opt::Opt() {
}

Opt::~Opt() {
}

bool Opt::registerOpt(int iId, const char *pszName, bool bIsRequired, bool bNeedsParam, const char *pszHelpText, const char *pszDefaultValue) {
  OptEntry entry;
  entry.m_iId = iId;
  entry.m_bIsRequired = bIsRequired;
  entry.m_bNeedsParam = bNeedsParam;
  entry.m_sParam = pszDefaultValue ? pszDefaultValue : "";
  entry.m_sHelpText = pszHelpText ? pszHelpText : "?";
  m_OptMap[pszName] = entry;
  return true;
}

void Opt::printHelp(FILE *pFile) const {
  if(!pFile) pFile = stdout;
  fprintf(pFile, "Syntax: %s", m_sAppName.c_str());

  std::map<std::string, OptEntry>::const_iterator it = m_OptMap.begin();
  while(it != m_OptMap.end()) {
    fputc(' ', pFile);
    if(!it->second.m_bIsRequired) fputc('[', pFile);
    fprintf(pFile, "%s", it->first.c_str());
    if(it->second.m_bNeedsParam) fprintf(pFile, " <?>");
    if(!it->second.m_bIsRequired) fputc(']', pFile);
    ++it;
  }
  fputc('\n', pFile);
  
  it = m_OptMap.begin();
  while(it != m_OptMap.end()) {
    fprintf(pFile, "%s\t%s\n", it->first.c_str(), it->second.m_sHelpText.c_str());
    ++it;
  }
}

bool Opt::parse(int argc, char *argv[]) {
  m_sAppName = argv[0];
  std::vector<std::string> args;
  for(int i = 1; i < argc; ++i) args.push_back(argv[i]);
  while(!args.empty()) {
    std::string sOpt = args.front();
    args.erase(args.begin());
    //printf("\"%s\"\n", sOpt.c_str());
    if(sOpt[0] == '-') { // Options starting with dash
      std::map<std::string, OptEntry>::iterator it = m_OptMap.find(sOpt);
      if(it == m_OptMap.end()) {
        fprintf(stderr, "Unknown option \"%s\"!\n", sOpt.c_str());
        return false;
      }
      it->second.m_bIsSet = true;
      if(it->second.m_bNeedsParam) {
        if(args.empty()) {
          fprintf(stderr, "Missing parameter for option \"%s\"!\n", sOpt.c_str());
          return false;
        }
        it->second.m_sParam = args.front();
        args.erase(args.begin());
      }
    }
    else if(sOpt[0] == '@') { // Options starting with @
      if(!loadFile(args, sOpt.substr(1))) return false;
    }
    else { // Options without dash-prefix
      bool bFound = false;
      std::map<std::string, OptEntry>::iterator it = m_OptMap.begin();
      while(it != m_OptMap.end()) {
        if(it->first[0] != '-' && !it->second.m_bIsSet) { // assign to first unset non-dash option
          it->second.m_sParam = sOpt;
          it->second.m_bIsSet = true;
          bFound = true;
          break;
        }
        ++it;
      }
      if(!bFound) {
        fprintf(stderr, "Unassignable option \"%s\"!\n", sOpt.c_str());
        return false;
      }
    }
  }

  bool bSuccess = true;
  std::map<std::string, OptEntry>::const_iterator it = m_OptMap.begin();
  while(it != m_OptMap.end()) {
    if(it->second.m_bIsRequired && !it->second.m_bIsSet) {
      fprintf(stderr, "Missing required option \"%s\"!\n", it->first.c_str());
      bSuccess = false;
    }
    ++it;
  }
  return bSuccess;
}

bool Opt::get(const char *pszOptName) const {
  std::map<std::string, OptEntry>::const_iterator it = m_OptMap.find(pszOptName);
  return it != m_OptMap.end() && it->second.m_bIsSet;
}

bool Opt::get(int iId) const {
  std::map<std::string, OptEntry>::const_iterator it = m_OptMap.begin();
  while(it != m_OptMap.end()) {
    if(it->second.m_iId == iId) return it->second.m_bIsSet;
    ++it;
  }
  return false;
}

const std::string &Opt::getString(const char *pszOptName) const {
  std::map<std::string, OptEntry>::const_iterator it = m_OptMap.find(pszOptName);
  return it != m_OptMap.end() ? it->second.m_sParam : m_csEmpty;
}

const std::string &Opt::getString(int iId) const {
  std::map<std::string, OptEntry>::const_iterator it = m_OptMap.begin();
  while(it != m_OptMap.end()) {
    if(it->second.m_iId == iId) return it->second.m_sParam;
    ++it;
  }
  return m_csEmpty;
}

int Opt::getInt(const char *pszOptName) const {
  return atoi(getString(pszOptName).c_str());
}

int Opt::getInt(int iId) const {
  return atoi(getString(iId).c_str());
}

double Opt::getDouble(const char *pszOptName) const {
  return atof(getString(pszOptName).c_str());
}

double Opt::getDouble(int iId) const {
  return atof(getString(iId).c_str());
}

void Opt::dump() const {
  std::map<std::string, OptEntry>::const_iterator it = m_OptMap.begin();
  while(it != m_OptMap.end()) {
    printf("\"%s\" %s \"%s\"\n", it->first.c_str(),
      it->second.m_bIsSet ? "set to" : "not set, default:",
      it->second.m_sParam.c_str());
    ++it;
  }
}

bool Opt::loadFile(std::vector<std::string> &args, std::string sFileName) {
  FILE *fp = fopen(sFileName.c_str(), "r");
  if(!fp) {
    perror(sFileName.c_str());
    return false;
  }
  char lineBuf[512];
  while(fgets(lineBuf, sizeof lineBuf, fp) != NULL) {
    char *p = strchr(lineBuf, '\n');
    if(p) *p = 0;
    if(strlen(lineBuf) > 0) args.push_back(lineBuf); // TODO: allow multiple arguments per line!
  }
  fclose(fp);
  return true;
}
