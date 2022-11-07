#include <stdhdr.h>
#pragma hdrstop

#include "Utilities/CVSFileInfo.h"
namespace {
  Utilities::CVSFileInfo moduleCVSFileInfo(
    "$RCSfile: CVSFileInfo.cpp,v $",
    "$Revision: 1.4 $",
    "$Date: 2002/04/07 13:43:08 $",
    "$Name: CLIB_1_0 $",
    __FILE__,
    __DATE__,
    __TIME__
  );
};

#include <string>
#include <Loki/Singleton.h>

using std::string;
using std::vector;


namespace Utilities {


  namespace {
    const char* CVSFilenamePrefix= "$RCSfile: ";
    const char* RevisionNumberPrefix= "$Revision: ";
    const char* CheckinDatePrefix= "$Date: ";
    const char* CVSTagPrefix= "$Name: ";
    string ExtractCVSFilename(const string&);
    string ExtractRevisionNumber(const string&);
    string ExtractCheckinDate(const string&);
    string ExtractCVSTag(const string&);
  }


  CVSFileInfo::CVSFileInfo(
    const string& __CVSFilename,
    const string& _revisionNumber,
    const string& _checkinDate,
    const string& __CVSTag,
    const string& _compileFilename,
    const string& _compileDate,
    const string& _compileTime)
  : _CVSFilename(ExtractCVSFilename(__CVSFilename)),
    revisionNumber(ExtractRevisionNumber(_revisionNumber)),
    checkinDate(ExtractCheckinDate(_checkinDate)),
    _CVSTag(ExtractCVSTag(__CVSTag)),
    compileFilename(_compileFilename),
    compileDate(_compileDate),
    compileTime(_compileTime)
  {
    FileInfoList().push_back(this);
  }


  vector<const CVSFileInfo*>& CVSFileInfo::FileInfoList()
  {
    typedef Loki::SingletonHolder<vector<const CVSFileInfo*> > Singleton;
    return Singleton::Instance();
  }


  namespace {
    inline string Erase(const string& prefix, const string& srceStr)
    {
      string str= srceStr;
      str.erase(0, prefix.size()); // remove "prefix"
      str.erase(str.size()-2, str.size()); // remove " $"
      return str;
    }


    inline string ExtractCVSFilename(const string& filenameStr)
    {
      return Erase(CVSFilenamePrefix, filenameStr);
    }


    inline string ExtractRevisionNumber(const string& revisionNumberStr)
    {
      return Erase(RevisionNumberPrefix, revisionNumberStr);
    }


    inline string ExtractCheckinDate(const string& checkinDateStr)
    {
      return Erase(CheckinDatePrefix, checkinDateStr);
    }


    inline string ExtractCVSTag(const string& CVSTagStr)
    {
      return Erase(CVSTagPrefix, CVSTagStr);
    }
  }


};
