#include "../freetype/freetype_rich_text.h"
#include "../application/logging.h"

#include <Windows.h>

#define IS_WHITESPACE_CHAR(ch) (ch == L'\n' || ch == L'\t' || ch == L'\r' || ch == L' ')

namespace snuffbox
{
	//-----------------------------------------------------------------------------------------------------------
	MarkupOperations RichTextParser::Parse(std::wstring* buffer)
	{
		MarkupOperations operations;
		wchar_t ch = L'';
		std::wstring current = L"";
		bool in_tag = false;

		for (int i = 0; i < buffer->size(); ++i)
		{
			ch = buffer->at(i);

			if (ch == L'[')
			{
				in_tag = true;
				if (current.size() > 0)
				{
					Markup markup;
					markup.is_default = true;
					markup.text = current;
					operations.push_back(markup);

					current.clear();
				}
				++i;

				if (i >= buffer->size())
				{
					return operations;
				}
				else
				{
					if (buffer->at(i) == L'/')
					{
						continue;
					}
				}
				std::wstring tag = GetTag(i, buffer);
				std::wstring to_close = L"[/" + tag + L"]";
				bool nested = false;
				Markup markup;

				if (tag == L"b")
				{
					markup.bold = true;
				}
				else if (tag == L"i")
				{
					markup.italic = true;
				}
				else if (ConsumeRaw(L"size", &tag))
				{
					RemoveWhitespaces(&tag);
					SplitValue(&tag);

					markup.do_size = true;
					markup.size = GetSize(&tag);

					to_close = L"[/size]";
				}
				else if (ConsumeRaw(L"colour", &tag))
				{
					RemoveWhitespaces(&tag);
					SplitValue(&tag);

					std::vector<float> colour = GetColour(&tag);
					markup.do_colour = true;
					
					for (int j = 0; j < colour.size(); ++j)
					{
						markup.colour[j] = colour[j];
					}

					to_close = L"[/colour]";
				}
				else if (ConsumeRaw(L"icon", &tag))
				{
					RemoveWhitespaces(&tag);
					SplitValue(&tag);

					markup.icon = true;
					markup.icon_path = tag;

					nested = true;

					operations.push_back(markup);
					in_tag = false;
				}
				else if (ConsumeRaw(L"font", &tag))
				{
					RemoveWhitespaces(&tag);
					SplitValue(&tag);

					markup.new_font = true;

					int size = WideCharToMultiByte(CP_UTF8, 0, tag.c_str(), -1, NULL, 0, 0, 0);

					char* multistr = new char[size];
					WideCharToMultiByte(CP_UTF8, 0, tag.c_str(), -1, multistr, size, 0, 0);

					markup.font = multistr;

					delete[] multistr;

					to_close = L"[/font]";
				}
				else
				{
					int size = WideCharToMultiByte(CP_UTF8, 0, tag.c_str(), -1, NULL, 0, 0, 0);

					char* multistr = new char[size];
					WideCharToMultiByte(CP_UTF8, 0, tag.c_str(), -1, multistr, size, 0, 0);

					SNUFF_ASSERT("Unknown rich text tag found! Aborting\n\n\t" + std::string(multistr), "RichTextParser::Parse");
				}

				if (nested == false)
				{
					std::wstring encapsulated = L"";
					while (i < buffer->size())
					{
						++i;
						if (ConsumeRawIndex(to_close, i, buffer))
						{
							operations.push_back(markup);

							if (Contains(L'[', &encapsulated))
							{
								MarkupOperations ops = Parse(&encapsulated);
								for (int i = 0; i < ops.size(); ++i)
								{
									operations.push_back(ops.at(i));
								}
							}
							else
							{
								Markup markup;
								markup.is_default = true;
								markup.text = encapsulated;
								operations.push_back(markup);
							}
							break;
						}
						if (i >= buffer->size())
						{

							int size = WideCharToMultiByte(CP_UTF8, 0, buffer->c_str(), -1, NULL, 0, 0, 0);

							char* multistr = new char[size];
							WideCharToMultiByte(CP_UTF8, 0, buffer->c_str(), -1, multistr, size, 0, 0);

							SNUFF_ASSERT("RichText formatting error! Aborting\n\n\t" + std::string(multistr), "RichTextParser::Parse");
						}
						encapsulated += buffer->at(i);
					}
				}
			}
			else if (ch == L']')
			{
				in_tag = false;
			}
			else if (in_tag == false)
			{
				current += ch;
			}
		}

		if (current.size() > 0)
		{
			Markup markup;
			markup.is_default = true;
			markup.text = current;
			operations.push_back(markup);
		}

		return operations;
	}

	//-----------------------------------------------------------------------------------------------------------
	bool RichTextParser::Consume(const std::wstring& to_consume, int& i, std::wstring* buffer)
	{
		int start = i;
		while (i - start < to_consume.size() && i - start < buffer->size())
		{
			if (to_consume.at(i - start) != buffer->at(i))
			{
				return false;
			}

			++i;
		}

		return true;
	}

	//-----------------------------------------------------------------------------------------------------------
	bool RichTextParser::ConsumeRaw(const std::wstring& to_consume, std::wstring* buffer)
	{
		int i = 0;
		while (i < to_consume.size() && i < buffer->size())
		{
			if (to_consume.at(i) != buffer->at(i))
			{
				return false;
			}

			++i;
		}

		return true;
	}

	//-----------------------------------------------------------------------------------------------------------
	bool RichTextParser::ConsumeRawIndex(const std::wstring& to_consume, const int& i, std::wstring* buffer)
	{
		int j = i;
		int start = i;
		while (j - start < to_consume.size() && j - start < buffer->size())
		{
			if (j >= buffer->size() || to_consume.at(j - start) != buffer->at(j))
			{
				return false;
			}

			++j;
		}

		return true;
	}

	//-----------------------------------------------------------------------------------------------------------
	bool RichTextParser::Contains(const wchar_t& value, std::wstring* buffer)
	{
		for (int i = 0; i < buffer->size(); ++i)
		{
			if (buffer->at(i) == value)
			{
				return true;
			}
		}

		return false;
	}

	//-----------------------------------------------------------------------------------------------------------
	std::wstring RichTextParser::GetTag(int& i, std::wstring* buffer)
	{
		std::wstring result;
		wchar_t ch = L'';

		while (i < buffer->size())
		{
			SkipWhitespaces(i, buffer);
			ch = buffer->at(i);

			if (ch == L']')
			{
				break;
			}

			result += ch;
			++i;
		}

		return result;
	}

	//-----------------------------------------------------------------------------------------------------------
	void RichTextParser::SkipWhitespaces(int& i, std::wstring* buffer)
	{
		while (IS_WHITESPACE_CHAR(buffer->at(i)))
		{
			++i;
		}
	}

	//-----------------------------------------------------------------------------------------------------------
	void RichTextParser::RemoveWhitespaces(std::wstring* buffer)
	{
		std::wstring result;
		wchar_t ch = L'';

		for (int i = 0; i < buffer->size(); ++i)
		{
			ch = buffer->at(i);
			if (!IS_WHITESPACE_CHAR(ch))
			{
				result += ch;
			}
		}

		*buffer = result;
	}

	//-----------------------------------------------------------------------------------------------------------
	void RichTextParser::SplitValue(std::wstring* buffer)
	{
		std::wstring result = L"";
		bool next = false;
		wchar_t ch = L'';
		for (int i = 0; i < buffer->size(); ++i)
		{
			ch = buffer->at(i);

			if (next == true)
			{
				result += ch;
			}
			if (ch == L'=')
			{
				next = true;
			}
		}

		*buffer = result;
	}

	//-----------------------------------------------------------------------------------------------------------
	std::vector<float> RichTextParser::GetColour(std::wstring* buffer)
	{
		std::wstring hex = L"0x" + *buffer;
		int colour = wcstoul(hex.c_str(), NULL, 0);
		std::vector<float> col;

		col.push_back(((colour >> 16) & 0xFF) / 255.0f);
		col.push_back(((colour >> 8) & 0xFF) / 255.0f);
		col.push_back(((colour) & 0xFF) / 255.0f);

		return col;
	}

	//-----------------------------------------------------------------------------------------------------------
	float RichTextParser::GetSize(std::wstring* buffer)
	{
		return wcstof(buffer->c_str(), NULL);
	}
}