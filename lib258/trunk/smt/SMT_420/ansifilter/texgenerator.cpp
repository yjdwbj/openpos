/***************************************************************************
                     texgenerator.cpp  -  description
                             -------------------

    copyright            : (C) 2008 by Andre Simon
    email                : andre.simon1@gmx.de
 ***************************************************************************/

/*
This file is part of ANSIFilter.

ANSIFilter is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

ANSIFilter is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with ANSIFilter.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <fstream>
#include <iostream>
#include <sstream>

#include "texgenerator.h"
#include "charcodes.h"
#include "version.h"

using namespace std;

namespace ansifilter {

TeXGenerator::TeXGenerator ():
  CodeGenerator(TEX),
  fileSuffix(".tex")
{
    /*This makes TeX to use every par it encounters (the \\leavevmode has
  no effect when TeX is in horizontal mode and when TeX is in vertical
  mode, it switches it to horizontal mode).*/
  newLineTag="\\leavevmode\\par\n";
  styleCommentOpen="%";
}

string TeXGenerator::getOpenTag()
{
	ostringstream os;
	os<< "{";

	if (elementStyle.isBold()) {
		os<< "\\bf";
	}
	if (elementStyle.isItalic()) {
		os<< "\\it";
	}

	os << "\\textColor{"
	   << elementStyle.getFgColour().getRed(TEX) << " "
	   << elementStyle.getFgColour().getGreen(TEX) << " "
	   << elementStyle.getFgColour().getBlue(TEX)
	   << " 0}";

	return os.str();
}

string TeXGenerator::getCloseTag()
{
	return "}";
}

string TeXGenerator::getGeneratorComment(){
 ostringstream s;
 s <<"% TeX generated by ansifilter "
   << ANSIFILTER_VERSION << ", " <<  ANSIFILTER_URL <<"\n";

 return s.str();
}

string TeXGenerator::getHeader()
{
  ostringstream os;
  os << styleCommentOpen <<docTitle << "\n"
     << "\\nopagenumbers\n"
     << "\\input colordvi\n";
  if (!styleSheetPath.empty()){
      os <<  "\\input "<<styleSheetPath<<"\n";
  }
  os << "{\n"
     << "\\tt\n";
  return os.str();
}

string TeXGenerator::getFooter()
{
  return "\n}\n\\bye\n" + getGeneratorComment();
}

void TeXGenerator::printBody()
{
  processRootState();
}

string TeXGenerator::maskCharacter(unsigned char c)
{
  switch (c)
    {
    case '{':
    case '}':
      {
        string m( "$\\" );
        m += c;
        m += '$';
        return m;
      }
      break;
    case '^':
      return "{\\bf\\^{}}";
      break;
    case '_':
      return "\\_{}";
      break;
    case '&':
    case '$':
    case '#':
    case '%':
      {
        string m( "\\" );
        m += c;
        return m;
      }
      break;
    case '\\':
      return "$\\backslash$";
      break;
    case ' ':
      return "\\ ";;
      break;
    case '+':
    case '-':
    case '<':
    case '>':
    case '=':
      {
        string m( "$\\mathord{" );
        m += c;
        m += "}$";
        return m;
      }
      break;
    case AUML_LC:
      return "\\\"a";
      break;
    case OUML_LC:
      return "\\\"o";
      break;
    case UUML_LC:
      return "\\\"u";
      break;
    case AUML_UC:
      return "\\\"A";
      break;
    case OUML_UC:
      return "\\\"O";
      break;
    case UUML_UC:
      return "\\\"U";
      break;
    case AACUTE_LC:
      return "\\'a";
      break;
    case EACUTE_LC:
      return "\\'e";
      break;
    case OACUTE_LC:
      return "\\'o";
      break;
    case UACUTE_LC:
      return "\\'u";
      break;
    case AGRAVE_LC:
      return "\\`a";
      break;
    case EGRAVE_LC:
      return "\\`e";
      break;
    case OGRAVE_LC:
      return "\\`o";
      break;
    case UGRAVE_LC:
      return "\\`u";
      break;
    case AACUTE_UC:
      return "\\'A";
      break;
    case EACUTE_UC:
      return "\\'E";
      break;
    case OACUTE_UC:
      return "\\'O";
      break;
    case UACUTE_UC:
      return "\\'U";
      break;
    case AGRAVE_UC:
      return "\\`A";
      break;
    case EGRAVE_UC:
      return "\\`E";
      break;
    case UGRAVE_UC:
      return "\\`O";
      break;
    case OGRAVE_UC:
      return "\\`U";
      break;
    case SZLIG:
      return "\\ss ";
      break;
      
    case '\t' : // see deletion of nonprintable chars below
      return "\t";
      break;

    default :
       if (c>0x1f ){ // printable?
	return string( 1, c );
      } else {
	return "";
      }
    }
}


}