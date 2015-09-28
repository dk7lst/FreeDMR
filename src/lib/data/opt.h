#include <string>
#include <map>

class Opt {
public:
  Opt();
  virtual ~Opt();

  bool registerOpt(const char *pszName, bool bIsRequired = true, bool bNeedsParam = false, const char *pszHelpText = NULL, const char *pszDefaultValue = NULL);

  void printHelp(FILE *pFile = NULL) const;

  bool parse(int argc, char *argv[]);

  bool get(const char *pszOptName) const;
  const std::string &getString(const char *pszOptName) const;
  int getInt(const char *pszOptName) const;
  double getDouble(const char *pszOptName) const;

  void dump() const;

protected:
  struct OptEntry {
    OptEntry() {
      m_bIsRequired = m_bNeedsParam = m_bIsSet = false;
    }

    bool m_bIsRequired;
    bool m_bNeedsParam;
    bool m_bIsSet;
    std::string m_sParam;
    std::string m_sHelpText;
  };

  std::string m_sAppName;
  std::map<std::string, OptEntry> m_OptMap;
};
