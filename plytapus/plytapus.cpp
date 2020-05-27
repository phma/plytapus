#include "plytapus_internal.h"

#include <fstream>
#include <string>
#include <cfloat>
#include "config.h"

#ifdef BIGENDIAN
#define NATIVE_ENDIAN BINARY_BIG_ENDIAN
#else
#define NATIVE_ENDIAN BINARY_LITTLE_ENDIAN
#endif

namespace plytapus
{

std::string ldecimal(double x,double toler)
{
  double x2;
  int h,i,iexp,chexp;
  size_t zpos;
  char *dotpos,*epos,*pLcNumeric;
  std::string ret,s,m,antissa,exponent,saveLcNumeric;
  char buffer[32],fmt[8];
  assert(toler>=0);
  pLcNumeric=setlocale(LC_NUMERIC,nullptr);
  if (pLcNumeric)
    saveLcNumeric=pLcNumeric;
  setlocale(LC_NUMERIC,"C");
  if (toler>0 && x!=0)
  {
    iexp=floor(log10(fabs(x/toler))-1);
    if (iexp<0)
      iexp=0;
  }
  else
    iexp=DBL_DIG-1;
  h=-1;
  i=iexp;
  while (true)
  {
    sprintf(fmt,"%%.%de",i);
    sprintf(buffer,fmt,x);
    x2=atof(buffer);
    if (h>0 && (fabs(x-x2)<=toler || i>=DBL_DIG+3))
      break;
    // GCC atof("inf")==0. MSVC atof("inf")=INFINITY.
    if (fabs(x-x2)>toler || std::isnan(x-x2) || i<=0)
      h=1;
    i+=h;
  }
  dotpos=strchr(buffer,'.');
  epos=strchr(buffer,'e');
  if (epos && !dotpos) // e.g. 2e+00 becomes 2.e+00
  {
    memmove(epos+1,epos,buffer+31-epos);
    dotpos=epos++;
    *dotpos='.';
  }
  if (dotpos && epos)
  {
    m=std::string(buffer,dotpos-buffer);
    antissa=std::string(dotpos+1,epos-dotpos-1);
    exponent=std::string(epos+1);
    if (m.length()>1)
    {
      s=m.substr(0,1);
      m.erase(0,1);
    }
    iexp=atoi(exponent.c_str());
    zpos=antissa.find_last_not_of('0');
    antissa.erase(zpos+1);
    iexp=stoi(exponent);
    if (iexp<0 && iexp>-5)
    {
      antissa=m+antissa;
      m="";
      iexp++;
    }
    if (iexp>0)
    {
      chexp=iexp;
      if (chexp>antissa.length())
	chexp=antissa.length();
      m+=antissa.substr(0,chexp);
      antissa.erase(0,chexp);
      iexp-=chexp;
    }
    while (iexp>-5 && iexp<0 && m.length()==0)
    {
      antissa="0"+antissa;
      iexp++;
    }
    while (iexp<3 && iexp>0 && antissa.length()==0)
    {
      m+='0';
      iexp--;
    }
    sprintf(buffer,"%d",iexp);
    exponent=buffer;
    ret=s+m;
    if (antissa.length())
      ret+='.'+antissa;
    if (iexp)
      ret+='e'+exponent;
  }
  else
    ret=buffer;
  setlocale(LC_NUMERIC,saveLcNumeric.c_str());
  return ret;
}

std::string ldecimal(float x,float toler)
{
  float x2;
  int h,i,iexp,chexp;
  size_t zpos;
  char *dotpos,*epos,*pLcNumeric;
  std::string ret,s,m,antissa,exponent,saveLcNumeric;
  char buffer[32],fmt[8];
  assert(toler>=0);
  pLcNumeric=setlocale(LC_NUMERIC,nullptr);
  if (pLcNumeric)
    saveLcNumeric=pLcNumeric;
  setlocale(LC_NUMERIC,"C");
  if (toler>0 && x!=0)
  {
    iexp=floor(log10(fabs(x/toler))-1);
    if (iexp<0)
      iexp=0;
  }
  else
    iexp=FLT_DIG-1;
  h=-1;
  i=iexp;
  while (true)
  {
    sprintf(fmt,"%%.%de",i);
    sprintf(buffer,fmt,x);
    x2=atof(buffer);
    if (h>0 && (fabs(x-x2)<=toler || i>=FLT_DIG+3))
      break;
    // GCC atof("inf")==0. MSVC atof("inf")=INFINITY.
    if (fabs(x-x2)>toler || std::isnan(x-x2) || i<=0)
      h=1;
    i+=h;
  }
  dotpos=strchr(buffer,'.');
  epos=strchr(buffer,'e');
  if (epos && !dotpos) // e.g. 2e+00 becomes 2.e+00
  {
    memmove(epos+1,epos,buffer+31-epos);
    dotpos=epos++;
    *dotpos='.';
  }
  if (dotpos && epos)
  {
    m=std::string(buffer,dotpos-buffer);
    antissa=std::string(dotpos+1,epos-dotpos-1);
    exponent=std::string(epos+1);
    if (m.length()>1)
    {
      s=m.substr(0,1);
      m.erase(0,1);
    }
    iexp=atoi(exponent.c_str());
    zpos=antissa.find_last_not_of('0');
    antissa.erase(zpos+1);
    iexp=stoi(exponent);
    if (iexp<0 && iexp>-5)
    {
      antissa=m+antissa;
      m="";
      iexp++;
    }
    if (iexp>0)
    {
      chexp=iexp;
      if (chexp>antissa.length())
	chexp=antissa.length();
      m+=antissa.substr(0,chexp);
      antissa.erase(0,chexp);
      iexp-=chexp;
    }
    while (iexp>-5 && iexp<0 && m.length()==0)
    {
      antissa="0"+antissa;
      iexp++;
    }
    while (iexp<3 && iexp>0 && antissa.length()==0)
    {
      m+='0';
      iexp--;
    }
    sprintf(buffer,"%d",iexp);
    exponent=buffer;
    ret=s+m;
    if (antissa.length())
      ret+='.'+antissa;
    if (iexp)
      ret+='e'+exponent;
  }
  else
    ret=buffer;
  setlocale(LC_NUMERIC,saveLcNumeric.c_str());
  return ret;
}

void endianflip(char *addr,int n)
{
  int i;
  for (i=0;i<n/2;i++)
  {
    addr[i]^=addr[n-1-i];
    addr[n-1-i]^=addr[i];
    addr[i]^=addr[n-1-i];
  }
}

void readEndian(std::ifstream &fs,char *buffer,int size,File::Format format)
{
  fs.read(buffer,size);
  if (format!=File::Format::NATIVE_ENDIAN)
    endianflip(buffer,size);
}

void writeEndian(std::ofstream &fs,char *buffer,int size,File::Format format)
{
  if (format!=File::Format::NATIVE_ENDIAN)
    endianflip(buffer,size);
  fs.write(buffer,size);
}

File::File(const PATH_STRING& filename)
	: m_filename(filename),
	m_parser(std::make_unique<FileParser>(filename))
{
}

File::~File() = default;

std::vector<Element> File::definitions() const 
{ 
	return m_parser->definitions(); 
}


void File::setElementReadCallback(std::string elementName, ElementReadCallback& readCallback)
{
	m_parser->setElementReadCallback(elementName, readCallback);
}

void File::read()
{ 
	m_parser->read(); 
};

void addElementDefinition(const textio::Tokenizer::TokenList& tokens, std::vector<ElementDefinition>& elementDefinitions)
{
	assert(std::string(tokens.at(0)) == "element");
	size_t startLine = 0;
	if (!elementDefinitions.empty())
	{
		const auto& previousElement = elementDefinitions.back();
		startLine = previousElement.startLine + previousElement.size;
	}
	ElementSize elementCount = std::stoul(tokens.at(2));
	elementDefinitions.emplace_back(tokens.at(1), elementCount, startLine);
}

void addProperty(const textio::Tokenizer::TokenList& tokens, ElementDefinition& elementDefinition)
{
	auto& properties = elementDefinition.properties;
	if (std::string(tokens.at(1)) == "list")
	{
		properties.emplace_back(tokens.back(), TYPE_MAP.at(tokens.at(3)), true, TYPE_MAP.at(tokens.at(2)));
	}
	else
	{
		properties.emplace_back(tokens.back(), TYPE_MAP.at(tokens.at(1)), false);
	}
}

Property PropertyDefinition::getProperty() const
{
	return Property(name, type, isList);
}

Element ElementDefinition::getElement() const
{
	std::vector<Property> properties;
	for (const auto& p : this->properties)
	{
		properties.emplace_back(p.getProperty());
	}
	return Element(name, size, properties);
}

FileParser::FileParser(const PATH_STRING& filename)
	: m_filename(filename),
	m_lineTokenizer(' '),
	m_lineReader(filename)
{
	readHeader();
}

FileParser::~FileParser() = default;

std::vector<Element> FileParser::definitions() const
{
	std::vector<Element> elements;
	for (const auto& e : m_elements)
	{
		elements.emplace_back(e.getElement());
	}
	return elements;
}

void FileParser::readHeader()
{
	// Read PLY magic number.
	std::string line = m_lineReader.getline();
	if (line != "ply")
	{
		throw std::runtime_error("Invalid file format.");
	}

	// Read file format.
	line = m_lineReader.getline();
	if (line == "format ascii 1.0")
	{
		m_format = File::Format::ASCII;
	}
	else if (line == "format binary_little_endian 1.0")
	{
		m_format = File::Format::BINARY_LITTLE_ENDIAN;
	}
	else if (line == "format binary_big_endian 1.0")
	{
		m_format = File::Format::BINARY_BIG_ENDIAN;
	}
	else
	{
		throw std::runtime_error("Unsupported PLY format : " + line);
	}

	// Read mesh elements properties.
	textio::SubString line_substring;
	line_substring = m_lineReader.getline();
	line = line_substring;
	textio::Tokenizer spaceTokenizer(' ');
	auto tokens = spaceTokenizer.tokenize(line);
	size_t startLine = 0;
	while (std::string(tokens.at(0)) != "end_header")
	{
		const std::string lineType = tokens.at(0);
		if (lineType == "element")
		{
			addElementDefinition(tokens, m_elements);
		}
		else if (lineType == "property")
		{
		  if (m_elements.size())
		    addProperty(tokens, m_elements.back());
		  else
		    throw std::runtime_error("Property with no preceding element.");
		}
		else
		{
			//throw std::runtime_error("Invalid header line.");
		}

		line_substring = m_lineReader.getline();
		line = line_substring;
		tokens = spaceTokenizer.tokenize(line);
	}
	
	m_dataOffset = m_lineReader.position(line_substring.end()) + 1;
}

void FileParser::setElementReadCallback(std::string elementName, ElementReadCallback& callback)
{
	m_readCallbackMap[elementName] = callback;
}

void FileParser::read()
{
	std::size_t totalLines = 0;
	for (auto& e : m_elements)
	{
		totalLines += e.size;
	}

	std::vector<std::shared_ptr<ElementBuffer>> buffers;
	for (auto& e : m_elements)
	{
		buffers.emplace_back(std::make_shared<ElementBuffer>(e));
	}

	std::size_t lineIndex = 0;
	std::size_t elementIndex = 0;
	ElementReadCallback readCallback = m_readCallbackMap.at(m_elements.at(elementIndex).name);
	auto& elementDefinition = m_elements.at(elementIndex);
	const std::size_t maxElementIndex = m_elements.size();
	
	std::shared_ptr<ElementBuffer> buffer = buffers[elementIndex];

	std::ifstream& filestream = m_lineReader.filestream();

	if (m_format == File::Format::BINARY_BIG_ENDIAN || m_format == File::Format::BINARY_LITTLE_ENDIAN)
	{
		filestream.clear();
		filestream.seekg(m_dataOffset);
	}

	while (lineIndex < totalLines)
	{
		const auto nextElementIndex = elementIndex + 1;
		if (nextElementIndex < maxElementIndex && lineIndex >= m_elements[nextElementIndex].startLine)
		{
			elementIndex = nextElementIndex;
			readCallback = m_readCallbackMap.at(m_elements.at(elementIndex).name);
			elementDefinition = m_elements.at(elementIndex);

			buffer = buffers[elementIndex];
		}

		if (m_format == File::Format::ASCII)
		{
			auto line = m_lineReader.getline();
			parseLine(line, elementDefinition, *buffer);
		}
		else {
			readBinaryElement(filestream, elementDefinition, *buffer, m_format);
		}
		
		readCallback(*buffer);
		++lineIndex;
	}
}

void FileParser::parseLine(const textio::SubString& line, const ElementDefinition& elementDefinition, ElementBuffer& elementBuffer)
{
	m_lineTokenizer.tokenize(line, m_tokens);
	const auto& properties = elementDefinition.properties;

	if (!properties.front().isList)
	{
		for (size_t i = 0; i < elementBuffer.size(); ++i)
		{
			properties[i].conversionFunction(m_tokens[i], elementBuffer[i]);
		}
	}
	else
	{
		const auto& conversionFunction = properties[0].conversionFunction;
		size_t listLength = std::stoi(m_tokens[0]);
		elementBuffer.reset(listLength);
		for (size_t i = 0; i < elementBuffer.size(); ++i)
		{
			conversionFunction(m_tokens[i+1], elementBuffer[i]);
		}
	}
}

void FileParser::readBinaryElement(std::ifstream& fs, const ElementDefinition& elementDefinition, ElementBuffer& elementBuffer, File::Format format)
{
	const auto& properties = elementDefinition.properties;
	const unsigned int MAX_PROPERTY_SIZE = 8;
	char buffer[MAX_PROPERTY_SIZE];

	if (!properties.front().isList)
	{
		for (size_t i = 0; i < elementBuffer.size(); ++i)
		{
			const auto size = TYPE_SIZE_MAP.at(properties[i].type);
			readEndian(fs, buffer, size, format);
			properties[i].castFunction(buffer, elementBuffer[i]);
		}
	}
	else
	{
		const auto lengthType = properties[0].listLengthType;
		const auto lengthTypeSize = TYPE_SIZE_MAP.at(lengthType);
		readEndian(fs, buffer, lengthTypeSize, format);
		size_t length = static_cast<size_t>(*buffer);
		elementBuffer.reset(length);

		const auto& castFunction = properties[0].castFunction;
		const auto size = TYPE_SIZE_MAP.at(properties[0].type);
		for (size_t i = 0; i < elementBuffer.size(); ++i)
		{
			readEndian(fs, buffer, size, format);
			castFunction(buffer, elementBuffer[i]);
		}
	}
}

ElementBuffer::ElementBuffer(const ElementDefinition& definition)
	: m_isList(false)
{
	auto& properties = definition.properties;
	for (auto& p : properties)
	{
		if (p.isList)
		{
			appendListProperty(p.type);
		}
		else
		{
			appendScalarProperty(p.type);
		}
	}

}

void ElementBuffer::reset(size_t size)
{
	if (properties.size() < size)
	{
		while (properties.size() < size)
		{
			properties.emplace_back(std::move(getScalarProperty(m_listType)));
		}
	}
	else
	{
		properties.resize(size);
	}
}

IScalarProperty& ElementBuffer::operator[](size_t index)
{
	return *properties[index];
}

void ElementBuffer::appendScalarProperty(Type type)
{
	std::unique_ptr<IScalarProperty> prop = getScalarProperty(type);
	properties.push_back(std::move(prop));
}

void ElementBuffer::appendListProperty(Type type)
{
	m_isList = true;
	m_listType = type;
}

std::unique_ptr<IScalarProperty> ElementBuffer::getScalarProperty(Type type)
{
	std::unique_ptr<IScalarProperty> prop;
	switch (type)
	{
	case Type::UCHAR: prop = std::make_unique<ScalarProperty<char>>();  break;
	case Type::INT: prop = std::make_unique<ScalarProperty<int>>(); break;
	case Type::FLOAT: prop = std::make_unique<ScalarProperty<float>>(); break;
	case Type::DOUBLE: prop = std::make_unique<ScalarProperty<double>>(); break;
	}
	return std::move(prop);
}

std::string formatString(File::Format format)
{
	switch (format)
	{
	case File::Format::ASCII: return "ascii";
	case File::Format::BINARY_BIG_ENDIAN: return "binary_big_endian";
	case File::Format::BINARY_LITTLE_ENDIAN: return "binary_little_endian";
	}
	return "";
}

std::string typeString(Type type)
{
	switch (type)
	{
	case Type::UCHAR: return "uchar";
	case Type::INT: return "int";
	case Type::FLOAT: return "float";
	case Type::DOUBLE: return "double";
	}
	return "";
}

void writePropertyDefinition(std::ofstream& file, const Property& propertyDefinition)
{
	if (propertyDefinition.isList)
	{
		file << "property list uchar ";
	}
	else
	{
		file << "property ";
	}
	file << typeString(propertyDefinition.type) << " " << propertyDefinition.name << '\n';
}

void writeElementDefinition(std::ofstream& file, const Element& elementDefinition)
{
	file << "element " << elementDefinition.name << " " << elementDefinition.size << '\n';
	for (const auto& prop : elementDefinition.properties)
	{
		writePropertyDefinition(file, prop);
	}
}

void writeTextProperties(std::ofstream& file, ElementBuffer& buffer, const ElementDefinition& elementDefinition)
{
	std::stringstream ss;
	if (elementDefinition.properties.front().isList)
	{
		file << buffer.size() << " ";
		auto& convert = elementDefinition.properties.front().writeConvertFunction;
		for (size_t i = 0; i < buffer.size(); ++i)
		{
			ss.clear();
			ss.str(std::string());
			file << convert(buffer[i], ss).str() << " ";
		}
	}
	else
	{
		for (size_t i = 0; i < buffer.size(); ++i)
		{
			auto& convert = elementDefinition.properties.at(i).writeConvertFunction;
			ss.clear();
			ss.str(std::string());
			file << convert(buffer[i], ss).str() << " ";
		}
	}
	file << '\n';
}

void writeBinaryProperties(std::ofstream& file, ElementBuffer& buffer, const ElementDefinition& elementDefinition, File::Format format)
{
	const unsigned int MAX_PROPERTY_SIZE = 8;
	char write_buffer[MAX_PROPERTY_SIZE];

	if (elementDefinition.properties.front().isList)
	{
		unsigned char list_size = static_cast<unsigned char>(buffer.size());
		writeEndian(file, reinterpret_cast<char*>(&list_size), sizeof(list_size), format);

		auto& cast = elementDefinition.properties.front().writeCastFunction;
		for (size_t i = 0; i < buffer.size(); ++i)
		{
			size_t write_size;
			cast(buffer[i], write_buffer, write_size);
			writeEndian(file, reinterpret_cast<char*>(write_buffer), write_size, format);
		}
	}
	else
	{
		for (size_t i = 0; i < buffer.size(); ++i)
		{
			auto& cast = elementDefinition.properties.at(i).writeCastFunction;
			size_t write_size;
			cast(buffer[i], write_buffer, write_size);
			writeEndian(file, reinterpret_cast<char*>(write_buffer), write_size, format);
		}
	}
}

void writeProperties(std::ofstream& file, ElementBuffer& buffer, size_t index, const ElementDefinition& elementDefinition, File::Format format, ElementWriteCallback& callback)
{
	callback(buffer, index);
	if (format == File::Format::ASCII)
	{
		writeTextProperties(file, buffer, elementDefinition);
	}
	else
	{
		writeBinaryProperties(file, buffer, elementDefinition, format);
	}
}

void writeElements(std::ofstream& file, const Element& elementDefinition, File::Format format, ElementWriteCallback& callback)
{
	const size_t size = elementDefinition.size;
	ElementBuffer buffer(elementDefinition);
	buffer.reset(elementDefinition.properties.size());
	for (size_t i = 0; i < size; ++i)
	{
		writeProperties(file, buffer, i, elementDefinition, format, callback);
	}
}

FileOut::FileOut(const PATH_STRING& filename, File::Format format)
	: m_filename(filename), m_format(format)
{
	createFile();
}

void FileOut::setElementsDefinition(const ElementsDefinition& definitions)
{
	m_definitions = definitions;
}

void FileOut::setElementWriteCallback(const std::string& elementName, ElementWriteCallback& writeCallback)
{
	m_writeCallbacks[elementName] = writeCallback;
}

void FileOut::write()
{
	writeHeader();
	writeData();
}

void FileOut::createFile()
{
	std::ofstream f(m_filename, std::ios::trunc);
	f.close();
}

void FileOut::writeHeader()
{
	std::ofstream file(m_filename, std::ios::out | std::ios::binary);

	file << "ply" << std::endl;
	file << "format " << formatString(m_format) << " 1.0" << std::endl;
	for (const auto& def : m_definitions)
	{
		writeElementDefinition(file, def);
	}
	file << "end_header" << std::endl;

	file.close();
}

void FileOut::writeData()
{
	std::ofstream file(m_filename, std::ios::out | std::ios::binary | std::ios::app);
	for (const auto& elem : m_definitions)
	{
		writeElements(file, elem, m_format, m_writeCallbacks[elem.name]);
	}
	file.close();
}

}
