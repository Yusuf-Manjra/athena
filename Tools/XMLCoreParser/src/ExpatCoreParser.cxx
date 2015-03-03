/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <expat.h>

#include <string>
#include <vector>
#include <iostream> 
#include <map> 
 
#include "XMLCoreParser/XMLCoreParser.h" 

#include "ExpatCoreParser.h"

class ExpatCoreParserDebugger
{ 
public: 
  static bool debug ()
      {
        static bool first = true;
        static bool debug_state = false;

        if (first)
          {
            first = false;

            if (::getenv ("XMLDEBUG") != 0)
              {
                debug_state = true;
              }
          }

        return (debug_state);
      }
}; 



#define BUFFSIZE 1000

void ExpatCoreParser::start (void* /*user_data*/, const char* el, const char** attr)
{
  ExpatCoreParser& me = instance ();
  me.do_start (el, attr);
}

void ExpatCoreParser::end (void* /*user_data*/, const char* el)
{
  ExpatCoreParser& me = instance ();
  me.do_end (el);
}

void ExpatCoreParser::char_data (void* /*user_data*/, const XML_Char* s, int len)
{
  ExpatCoreParser& me = instance ();
  me.do_char_data (s, len);
}

void ExpatCoreParser::default_handler (void* /*user_data*/, const XML_Char* s, int len)
{
  ExpatCoreParser& me = instance ();
  me.do_default_handler (s, len);
}

void ExpatCoreParser::comment (void* /*userData*/, const XML_Char* s)
{
  ExpatCoreParser& me = instance ();
  me.do_comment (s);
}

int ExpatCoreParser::external_entity (XML_Parser parser,
				      const XML_Char* context,
				      const XML_Char* /*base*/,
				      const XML_Char* systemId,
				      const XML_Char* /*publicId*/)
{
  ExpatCoreParser& me = instance ();
  return (me.do_external_entity (parser, context, systemId));
}
  
void ExpatCoreParser::register_external_entity (const std::string& name, const std::string& file_name) 
{
  if (ExpatCoreParserDebugger::debug ())
    {
      std::cout << "ExpatCoreParser::register_external_entity> name=" << name
		<< " file_name=" << file_name << std::endl; 
    }

  ExpatCoreParser& me = instance ();
  return (me.do_register_external_entity (name, file_name));
} 
  
void ExpatCoreParser::register_text_entity (const std::string& name, const std::string& text) 
{
  if (ExpatCoreParserDebugger::debug ())
    {
      std::cout << "ExpatCoreParser::register_text_entity> name=" << name
		<< std::endl; 
    }

  ExpatCoreParser& me = instance ();
  return (me.do_register_text_entity (name, text));
} 

void ExpatCoreParser::entity (void* /*userData*/,
			      const XML_Char* entityName,
			      int is_parameter_entity,
			      const XML_Char* value,
			      int value_length,
			      const XML_Char* base,
			      const XML_Char* systemId,
			      const XML_Char* publicId,
			      const XML_Char* /*notationName*/)
{
  if (!base) base = "none";
  if (!systemId) systemId = "none";
  if (!publicId) publicId = "none";
  
  std::string temp = "none";
  
  if (value)
    {
      char* t = (char*) malloc (value_length + 1);
      strncpy (t, value, value_length);
      t[value_length] = 0;
      
      temp = (const char*) t;
      
      free (t);
    }
  
  if (ExpatCoreParserDebugger::debug ())
    {
      //XML_Parser p = (XML_Parser) userData;
      
      //int ln = XML_GetCurrentLineNumber (p);
      
      std::cout << "ExpatCoreParser::entity[" << 0 << "]> " << entityName;
      std::cout << " is_parameter=" << is_parameter_entity;
      std::cout << " value=[" << temp << "]";
      std::cout << " base=[" << base << "]";
      std::cout << " systemId=[" << systemId << "]";
      std::cout << " publicId=[" << publicId << "]";
      std::cout << std::endl;
    }
}

CoreParser::DOMNode* ExpatCoreParser::get_document ()
{
  return (top);
}

ExpatCoreParser::ExpatCoreParser ()
{
  top = 0;
  last = 0;
}

void ExpatCoreParser::do_start (const char* el, const char** attr)
{
  int i;
  
  std::map <std::string, std::string> a;
  
  if (top == 0) 
    {
      top = new CoreParser::DOMNode ();
      last = top;
    }

  CoreParser::DOMNode* node = new CoreParser::DOMNode (CoreParser::DOMNode::ELEMENT_NODE, el, last);
  
  if (ExpatCoreParserDebugger::debug ())
    {
      std::cout << "ExpatCoreParser::do_start> el=" << el << " top=" << top << " last=" << last << " node=" << node << std::endl; 
    }
  
  last = node;
  
  for (i = 0; attr[i]; i += 2) 
    {
      const char* name = attr[i];
      const char* value = attr[i+1];
      
      node->m_attributes[name] = value;
    }
}

void ExpatCoreParser::do_end (const char* el)
{
  if (ExpatCoreParserDebugger::debug ())
    {
      std::cout << "ExpatCoreParser::do_end> el=" << el << std::endl; 
    }
  
  last = last->m_parent;
}

void ExpatCoreParser::do_char_data (const XML_Char* s, int len)
{
  if (len == 0) return;

  while ((len > 0) && (s[len-1] == '\n')) len--;
  
  if (len == 0) return;

  char* temp = (char*) malloc (len + 1);
  strncpy (temp, s, len);
  temp[len] = 0;
  
  if (ExpatCoreParserDebugger::debug ())
    {
      std::cout << "ExpatCoreParser::do_char_data> [" << temp << "]" << std::endl;
    }
  
  free (temp);
}

void ExpatCoreParser::do_default_handler (const XML_Char* s, int len)
{
  if (len == 0) return;

  while ((len > 0) && (s[len-1] == '\n')) len--;
  
  if (len == 0) return;

  char* temp = (char*) malloc (len + 1);
  strncpy (temp, s, len);
  temp[len] = 0;
  
  if (ExpatCoreParserDebugger::debug ())
    {
      std::cout << "ExpatCoreParser::do_default_handler> [" << temp << "]" << std::endl;
    }
  
  free (temp);
}
  
void ExpatCoreParser::do_comment (const XML_Char* s)
{
  if (top == 0) 
    {
      top = new CoreParser::DOMNode ();
      last = top;
    }

  CoreParser::DOMNode* node = new CoreParser::DOMNode (CoreParser::DOMNode::COMMENT_NODE, s, last);
  
  if (ExpatCoreParserDebugger::debug ())
    {
      std::cout << "ExpatCoreParser::do_comment> s=" << s << " top=" << top << " last=" << last << " node=" << node << std::endl; 
    }
}

int ExpatCoreParser::generic_parse (XML_Parser p, const std::string& file_name)
{
  FILE* docfd;
  
  if (ExpatCoreParserDebugger::debug ())
    {
      std::cout << "ExpatCoreParser::generic_parse> file_name=" << file_name << " prefix=" << prefix << std::endl; 
    }
  
  std::string temp_name = file_name;
  
  docfd = fopen (temp_name.c_str (), "r");
  if (!docfd)
    {
      temp_name = prefix + "/" + file_name;
      
      docfd = fopen (temp_name.c_str (), "r");
    }
  
  if (docfd == 0)
    {
      const char* xmlpathenv = ::getenv ("XMLPATH");
      
      if (xmlpathenv == 0) return (0);
      
      std::string xmlpath = xmlpathenv;
      
      std::string::size_type pos = 0;
      
      while (pos != std::string::npos)
	{
	  std::string::size_type sep = xmlpath.find (":", pos);
	  
	  if (sep == std::string::npos)
	    {
	      temp_name = xmlpath.substr (pos);
	      pos = std::string::npos;
	    }
	  else
	    {
	      temp_name = xmlpath.substr (pos, sep - pos);
	      pos = sep + 1;
	    }
	  
	  if (temp_name == "") continue;

	  std::string last_temp_name = temp_name;
	  
	  temp_name += "/";
	  temp_name += file_name;
	  
	  if (ExpatCoreParserDebugger::debug ())
	    {
	      std::cout << "ExpatCoreParser::generic_parse> file_name=" << file_name
			<< " xmlpath=" << xmlpath
			<< " temp_name=" << temp_name
			<< std::endl; 
	    }
	  
	  docfd = fopen (temp_name.c_str (), "r");
	  if (docfd != 0) break;

	  // Test whether prefix is a relative path and if so use it
	  if (prefix != "" && '/' != prefix[0]) {

	      temp_name =  last_temp_name;
	      temp_name += "/";
	      temp_name += prefix;
	      temp_name += "/";
	      temp_name += file_name;
	  
	      if (ExpatCoreParserDebugger::debug ()) 
		{
		  std::cout << "ExpatCoreParser::generic_parse> file_name=" << file_name
			    << " xmlpath=" << xmlpath
			    << " temp_name=" << temp_name
			    << std::endl; 
		}
	  
	      docfd = fopen (temp_name.c_str (), "r");
	      if (docfd != 0) break;
	  }
	}
    }
  
  if (docfd == 0)
    {
      return (0);
    }
  
  int result = 1;
  
  XML_SetParamEntityParsing (p, XML_PARAM_ENTITY_PARSING_ALWAYS);
  XML_SetElementHandler (p, start, end);
  XML_SetCharacterDataHandler (p, char_data);
  XML_SetExternalEntityRefHandler (p, external_entity);
  XML_SetCommentHandler (p, comment);
  //XML_SetDefaultHandlerExpand (p, default_handler);
  //XML_SetEntityDeclHandler (p, entity);
  //XML_UseParserAsHandlerArg (p);
  
  if (ExpatCoreParserDebugger::debug ())
    {
      std::cout << "ExpatCoreParser::generic_parse> starting" << std::endl;
    }
  
  for (;;) 
    {
      int items;
      
      char* buff = (char*) XML_GetBuffer (p, BUFFSIZE + 1);
      
      items = fread (buff, 1, BUFFSIZE, docfd);
      
      if (ferror (docfd))
	{
	  std::cout << "Read error" << std::endl;
	  result = 0;
	  break;
	}
      
      int done = feof (docfd);
      
      buff[items] = 0;
      
      //std::cout << "2) read=" << items << " [" << buff << "] " << done << std::endl;
      
      if (XML_ParseBuffer (p, BUFFSIZE, done) == XML_STATUS_ERROR) 
	{
	  if (!done)
	    {
	      std::cout << "ExpatCoreParser::Parse error at line " << XML_GetCurrentLineNumber(p)
			<< " of file " << file_name
			<< ":" << std::endl
			<< XML_ErrorString (XML_GetErrorCode(p)) << std::endl;
	      result = 0;
	    }
	  /* handle parse error */
	}
      if (done) break;
    }  
  
  fclose (docfd);
  
  return (result);
}

int ExpatCoreParser::generic_text_parse (XML_Parser p, const std::string& text)
{
  if (ExpatCoreParserDebugger::debug ())
    {
      std::cout << "ExpatCoreParser::generic_text_parse> " << std::endl; 
    }
  
  int result = 1;
  
  XML_SetParamEntityParsing (p, XML_PARAM_ENTITY_PARSING_ALWAYS);
  XML_SetElementHandler (p, start, end);
  XML_SetCharacterDataHandler (p, char_data);
  XML_SetExternalEntityRefHandler (p, external_entity);
  XML_SetCommentHandler (p, comment);
  //XML_SetDefaultHandlerExpand (p, default_handler);
  //XML_SetEntityDeclHandler (p, entity);
  //XML_UseParserAsHandlerArg (p);
  
  if (ExpatCoreParserDebugger::debug ())
    {
      std::cout << "ExpatCoreParser::generic_text_parse> starting" << std::endl;
    }
  
  for (;;) 
    {
      char* buff = (char*) XML_GetBuffer (p, text.size() + 1);
      strcpy (buff, text.c_str ());

      int done = 1;

      if (XML_ParseBuffer (p, text.size(), done) == XML_STATUS_ERROR) 
	{
            std::cout << "ExpatCoreParser::Parse error at line " << XML_GetCurrentLineNumber(p)
                      << ":" << std::endl
                      << XML_ErrorString (XML_GetErrorCode(p)) << std::endl;
            result = 0;
	  /* handle parse error */
	}
      if (done) break;
    }  

  return (result);
}

int ExpatCoreParser::do_external_entity (XML_Parser parser,
					 const XML_Char* context,
					 const XML_Char* systemId)
{
  std::string context_str;
  
  if (context == 0) context_str = "none";
  else context_str = context;

  bool done = false;
  int status = 0;

  if (context != 0)
    {
      std::string replacement = find_text_entity (context_str);

      if (replacement != "")
	{
	  if (ExpatCoreParserDebugger::debug ())
	    {
	      std::cout << "ExpatCoreParser::do_external_entity> "
			<< " context=[" << context_str << "]"
			<< " replacement=[" << replacement << "]"
			<< std::endl;
	    }
	  
	  XML_Parser p = XML_ExternalEntityParserCreate (parser, context, NULL);
	  
	  status = generic_text_parse (p, replacement);
	  
	  XML_ParserFree (p);
	  
	  done = true;
	}
    }

  if (!done)
    {
      std::string replacement = find_external_entity (context_str);
  
      if (replacement == "NULL")
	{
	  return (1);
	}

      if (replacement != "")
	{
	  systemId = replacement.c_str ();
	}

      if (ExpatCoreParserDebugger::debug ())
	{
	  std::cout << "ExpatCoreParser::do_external_entity> "
		    << " context=[" << context_str << "]"
		    << " systemId=[" << systemId << "]"
		    << " replacement=[" << replacement << "]"
		    << std::endl;
	}
      
      XML_Parser p = XML_ExternalEntityParserCreate (parser, context, NULL);
      
      status = generic_parse (p, systemId);
  
      XML_ParserFree (p);
    }


  return (status);
}

void ExpatCoreParser::do_register_external_entity (const std::string& name, const std::string& file_name) 
{
  if (ExpatCoreParserDebugger::debug ())
    {
      std::cout << "ExpatCoreParser::do_register_external_entity> name=" << name
		<< " file_name=" << file_name << std::endl; 
    }
  
  m_entities[name] = file_name; 
} 

void ExpatCoreParser::do_register_text_entity (const std::string& name, const std::string& text) 
{
  if (ExpatCoreParserDebugger::debug ())
    {
      std::cout << "ExpatCoreParser::do_register_text_entity> name=" << name
		<< std::endl; 
    }
  
  m_text_entities[name] = text; 
} 

const std::string& ExpatCoreParser::find_external_entity (const std::string& name) const
{ 
  ExternalEntityMap::const_iterator it; 
  
  it = m_entities.find (name); 
  
  if (it == m_entities.end ()) 
    {
      static const std::string empty;
      return (empty); 
    }
  else 
    { 
      return ((*it).second); 
    } 
} 

const std::string& ExpatCoreParser::find_text_entity (const std::string& name) const
{ 
  ExternalEntityMap::const_iterator it = m_text_entities.begin (); 
  
  it = m_text_entities.find (name); 

  if (it == m_text_entities.end ()) 
    {
      static const std::string empty;
      return (empty); 
    }
  else 
    { 
      return ((*it).second); 
    } 
} 

ExpatCoreParser& ExpatCoreParser::instance ()
{
  static ExpatCoreParser me;
  
  return (me);
}

CoreParser::DOMNode* ExpatCoreParser::parse (const std::string& file_name)
{
  static ExpatCoreParser& me = instance ();

  std::string name = file_name;

  std::string::size_type pos = file_name.rfind ('/');
  if (pos != std::string::npos)
    {
      me.prefix = file_name.substr (0, pos);
      name = file_name.substr (pos + 1);
    }
  else
    {
      me.prefix = "";
    }

  XML_Parser p = XML_ParserCreate (NULL);

  if (!p) 
    {
      std::cout << "ExpatCoreParser::Couldn't allocate memory for parser" << std::endl;
     exit(-1);
    }

  me.clean ();

  me.generic_parse (p, name);

  XML_ParserFree (p);

  return (me.get_document ());
}

void ExpatCoreParser::clean ()
{
  last = 0;

  if (top != 0)
    {
      delete top;
      top = 0;
    }
}
