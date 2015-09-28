#include <stdlib.h>
#include <stdio.h>
#include "opt.h"

Opt::Opt() {
}

Opt::~Opt() {
}

bool Opt::registerOpt(const char *pszName, bool bIsRequired, bool bNeedsParam, const char *pszHelpText, const char *pszDefaultValue) {
  OptEntry entry;
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
  const char * const*paszNextArg = argv + 1;
  int iNumArgLeft = argc - 1;
  while(iNumArgLeft--) {
    const char *pszOpt = *paszNextArg++;
    //printf("\"%s\"\n", pszOpt);
    if(*pszOpt == '-') {
      std::map<std::string, OptEntry>::iterator it = m_OptMap.find(pszOpt);
      if(it == m_OptMap.end()) {
        fprintf(stderr, "Unknown option \"%s\"!\n", pszOpt);
        return false;
      }
      it->second.m_bIsSet = true;
      if(it->second.m_bNeedsParam) {
        if(!iNumArgLeft) {
          fprintf(stderr, "Missing parameter for option \"%s\"!\n", pszOpt);
          return false;
        }
        it->second.m_sParam = *paszNextArg++;
        --iNumArgLeft;
      }
    }
    else {
      bool bFound = false;
      std::map<std::string, OptEntry>::iterator it = m_OptMap.begin();
      while(it != m_OptMap.end()) {
        if(it->first[0] != '-' && !it->second.m_bIsSet) {
          it->second.m_sParam = pszOpt;
          it->second.m_bIsSet = true;
          bFound = true;
          break;
        }
        ++it;
      }
      if(!bFound) {
        fprintf(stderr, "Unassignable option \"%s\"!\n", pszOpt);
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

const std::string &Opt::getString(const char *pszOptName) const {
  std::map<std::string, OptEntry>::const_iterator it = m_OptMap.find(pszOptName);
  return it != m_OptMap.end() ? it->second.m_sParam : "";
}

int Opt::getInt(const char *pszOptName) const {
  return atoi(getString(pszOptName).c_str());
}

double Opt::getDouble(const char *pszOptName) const {
  return atof(getString(pszOptName).c_str());
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
