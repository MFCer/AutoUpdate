#pragma once

#include <map>
using namespace std;

#define MG_SAFE_DELETE(ptr) {if (ptr) { delete (ptr); (ptr) = NULL; }}
#define MG_SAFE_DELETEA(pBase) {if (pBase) { delete [] (pBase); (pBase) = NULL; }}

#define IFO_FILE_NAME					_T("Config")
#define IFO_FILE_EXT					_T("ini")

#define IFO_SECTION_GENERAL				_T("General")
#define IFO_SG_LANG_CNT					_T("LangCnt")

#define CFG_SECTION_CONFIG				_T("Config")
#define CFG_SC_PRODUCT_NAME				_T("ProductName")
#define CFG_SC_WORKING_DIR				_T("WorkingDirectory")
#define CFG_SC_UPDATE_URL				_T("UpdateUrl")

// 更新说明文件XML头尾
#define XML_UH_HEADER					"<?xml version=\"1.0\" encoding=\"GB2312\" ?> \r\n<xmlRoot keyname=\"UpdateHint\">\r\n"
#define XML_UH_TAIL						"</xmlRoot>\r\n"

#define XML_UH_PRODUCT_HEADER			"\t<Product productname=\"%s\" createdate=\"%s\">\r\n"
#define XML_UH_PRODUCT_TAIL				"\t</Product>\r\n"

#define XML_UH_DESCRIPTION_HEADER		"\t\t<Description>\r\n"
#define XML_UH_DESCRIPTION_TAIL			"\t\t</Description>\r\n"

// 更新文件列表XML头尾
#define XML_FL_HEADER					"<?xml version=\"1.0\" encoding=\"GB2312\" ?> \r\n<xmlRoot keyname=\"FileList\">\r\n"
#define XML_FL_TAIL						"</xmlRoot>\r\n"

#define XML_FL_PRODUCT_HEADER			"\t<Product productname=\"%s\" createdate=\"%s\" filenum=\"%d\" url=\"%s\">\r\n"
#define XML_FL_PRODUCT_TAIL				"\t</Product>\r\n"

#define XML_FL_FILE_HEADER				"\t\t<F>\r\n"
#define XML_FL_FILE_TAIL				"\t\t</F>\r\n"

#define XML_FL_NODE_FILENAME			"\t\t\t<N>%s</N>\r\n"
#define XML_FL_NODE_FILELEN				"\t\t\t<L>%s</L>\r\n"
#define XML_FL_NODE_FILEVER				"\t\t\t<V>%s</V>\r\n"
#define XML_FL_NODE_EXECUTE				"\t\t\t<E>%s</E>\r\n"
#define XML_FL_NODE_OVERWRITE			"\t\t\t<O>%s</O>\r\n"
#define XML_FL_NODE_REGISTER			"\t\t\t<R>%s</R>\r\n"
#define XML_FL_NODE_DECOMPRESS			"\t\t\t<D>%s</D>\r\n"
#define XML_FL_NODE_UPDATENOTEXIST		"\t\t\t<U>%s</U>\r\n"
#define XML_FL_NODE_FILEMD5				"\t\t\t<M>%s</M>\r\n"
#define XML_FL_NODE_LANGID				"\t\t\t<I>%d</I>\r\n"
#define XML_FL_NODE_ORIGINALNAME		"\t\t\t<ON>%s</ON>\r\n"