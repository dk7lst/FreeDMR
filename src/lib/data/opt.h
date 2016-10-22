#pragma once
#include <string>
#include <vector>
#include <map>

class Opt {
public:
  Opt();
  virtual ~Opt();

  bool registerOpt(int iId, const char *pszName, bool bIsRequired = true, bool bNeedsParam = false, const char *pszHelpText = NULL, const char *pszDefaultValue = NULL);

  void printHelp(FILE *pFile = NULL) const;

  bool parse(int argc, char *argv[]);

  bool get(const char *pszOptName) const;
  bool get(int iId) const;

  const std::string &getString(const char *pszOptName) const;
  const std::string &getString(int iId) const;

  int getInt(const char *pszOptName) const;
  int getInt(int iId) const;

  double getDouble(const char *pszOptName) const;
  double getDouble(int iId) const;

  void dump() const;

protected:
  struct OptEntry {
    OptEntry() {
      m_iId = -1;
      m_bIsRequired = m_bNeedsParam = m_bIsSet = false;
    }

    int m_iId;
    bool m_bIsRequired;
    bool m_bNeedsParam;
    bool m_bIsSet;
    std::string m_sParam;
    std::string m_sHelpText;
  };

  bool loadFile(std::vector<std::string> &args, std::string sFileName);

  std::string m_sAppName;
  std::map<std::string, OptEntry> m_OptMap;
  const std::string m_csEmpty;
};
