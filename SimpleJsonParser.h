/*

		 ###################################
		 ###################################
			     JsonParser.h
		 ###################################
		 ###################################

*/
#ifndef JSON_PARSER_H
#define JSON_PARSER_H

typedef enum JsonTokenType
{
    JSON_TOKEN_TYPE_NONE = 0,
    JSON_TOKEN_TYPE_KEYVALUE_SEPARATOR,
    JSON_TOKEN_TYPE_STRING,
    JSON_TOKEN_TYPE_INT_NUMBER,
    JSON_TOKEN_TYPE_FLOAT_NUMBER,
    JSON_TOKEN_TYPE_BOOL_TRUE,
    JSON_TOKEN_TYPE_BOOL_FALSE,
    JSON_TOKEN_TYPE_NULL,
    JSON_TOKEN_TYPE_OBJECT_OPEN_BRACKET,
    JSON_TOKEN_TYPE_OBJECT_CLOSE_BRACKET,
    JSON_TOKEN_TYPE_ARRAY_OPEN_BRACKET,
    JSON_TOKEN_TYPE_ARRAY_CLOSE_BRACKET,
    JSON_TOKEN_TYPE_COMMA,

    JSON_TOKEN_TYPE_COUNT
} JsonTokenType;
typedef struct JsonToken
{
    JsonTokenType Type;
    void* Data;
} JsonToken;

typedef enum JsonValueType
{
    JSON_VALUE_TYPE_INT,
    JSON_VALUE_TYPE_FLOAT,
    JSON_VALUE_TYPE_BOOL,
    JSON_VALUE_TYPE_NULL,
    JSON_VALUE_TYPE_STRING,
    JSON_VALUE_TYPE_I32_ARRAY,
    JSON_VALUE_TYPE_F32_ARRAY,
    JSON_VALUE_TYPE_STRING_ARRAY,
    JSON_VALUE_TYPE_OBJECT
} JsonValueType;
typedef struct JsonValue
{
    JsonValueType Type;
    void* Data;
} JsonValue;
typedef struct JsonObject
{
    const char** Keys;
    JsonValue* Values;
} JsonObject;

typedef struct JsonParser
{
    char* SourceContent;
    JsonToken* Tokens;
    JsonObject* Object;
} JsonParser;

/*
  CREATE

  void json_add_key_value(JsonObject* object, char* key, void* data)
  {
  array_push(object->Keys, key);
  array_push(object->Values, data);
  }

*/

#define JSON_INT(num)				\
    ({						\
	i32* alloc = memory_allocate_type(i32);	\
	*alloc = num;				\
	JsonValue value;			\
	value.Data = alloc;			\
	value.Type = JSON_VALUE_TYPE_INT;	\
	value;					\
    })
#define JSON_FLOAT(num)				\
    ({						\
	f32* alloc = memory_allocate_type(f32);	\
	*alloc = num;				\
	JsonValue value;			\
	value.Data = alloc;			\
	value.Type = JSON_VALUE_TYPE_FLOAT;	\
	value;					\
    })
#define JSON_STRING(str)			\
    ({						\
	char* sstr = vstring(str);		\
	JsonValue value;			\
	value.Data = sstr;			\
	value.Type = JSON_VALUE_TYPE_STRING;	\
	value;					\
    })
#define JSON_TRUE(str)				\
    ({						\
	JSON_STRING("TRUE");			\
    })
#define JSON_FALSE(str)				\
    ({						\
	JSON_STRING("FALSE");			\
    })
#define JSON_NULL()				\
    ({						\
	JsonValue value;			\
	value.Data = (void*) istring("NULL");	\
	value.Type = JSON_VALUE_TYPE_NULL;	\
	value;					\
    })
#define JSON_I32_ARRAY(arr)			\
    ({						\
	JsonValue value;			\
	value.Data = arr;			\
	value.Type = JSON_VALUE_TYPE_I32_ARRAY;	\
	value;					\
    })
#define JSON_F32_ARRAY(arr)			\
    ({						\
	JsonValue value;			\
	value.Data = arr;			\
	value.Type = JSON_VALUE_TYPE_F32_ARRAY;	\
	value;					\
    })
//TODO(bies): optimize this
#define JSON_F32_ARRAY_NEW(arr, count)		\
    ({						\
	i32 i;					\
	f32* oldArr = (f32*)arr;		\
	f32* newArr = NULL;			\
	array_reserve(newArr, count);		\
	for(i = 0; i < count; ++i)		\
	{					\
	    array_push(newArr, oldArr[i]);	\
	}					\
	JsonValue value;			\
	value.Data = newArr;			\
	value.Type = JSON_VALUE_TYPE_F32_ARRAY;	\
	value;					\
    })
#define JSON_STRING_ARRAY(arr)				\
    ({							\
	JsonValue value;				\
	value.Data = arr;				\
	value.Type = JSON_VALUE_TYPE_STRING_ARRAY;	\
	value;						\
    })
#define JSON_OBJECT(obj)			\
    ({						\
	JsonValue value;			\
	value.Data = obj;			\
	value.Type = JSON_VALUE_TYPE_OBJECT;	\
	value;					\
    })

/*
  PUBLIC JSON UTILS
*/
static const char* json_token_to_string(JsonToken token);
static char* json_tokens_to_string(JsonToken* tokens);
static void json_object_free(JsonObject* obj);

force_inline JsonObject*
json_object_create()
{
    JsonObject* obj = memory_allocate_type(JsonObject);
    obj->Keys = NULL;
    obj->Values = NULL;
    return obj;
}

force_inline void
json_object_destroy(JsonObject* obj)
{
    i32 i, count = array_count(obj->Values);
    for (i = 0; i < count; ++i)
    {
	JsonValue value = obj->Values[i];
	if (value.Type == JSON_VALUE_TYPE_OBJECT)
	{
	    json_object_free((JsonObject*)value.Data);
	}
	else if (value.Type == JSON_VALUE_TYPE_I32_ARRAY
		 || value.Type == JSON_VALUE_TYPE_F32_ARRAY
		 || value.Type == JSON_VALUE_TYPE_STRING_ARRAY)
	{
	    array_free(value.Data);
	}
    }

    memory_free(obj);
}

typedef enum JsonLetterType
{
    JSON_LETTER_TYPE_NONE = 0,
    JSON_LETTER_TYPE_OPEN_BRACKET,
    JSON_LETTER_TYPE_CLOSE_BRACKET,
    JSON_LETTER_TYPE_OPEN_STRING
} JsonLetterType;

static const char* TrueKeyWord;
static const char* FalseKeyWord;
static const char* NullKeyWord;

force_inline void
_register_keywords()
{
    TrueKeyWord = istring("true");
    FalseKeyWord = istring("false");
    NullKeyWord = istring("null");
}

force_inline void
_skip_spaces(char* stream)
{
    while (*stream == ' ' && *stream == '\n' && *stream == '\t' && *stream == '\r')
    {
	++stream;
    }
}

force_inline i32
_json_is_array_of_type(JsonToken* tokens, JsonTokenType type, i32 index)
{
    i32 count = array_count(tokens);
    while (1)
    {
	JsonToken token = tokens[index];
	++index;

	if (token.Type == JSON_TOKEN_TYPE_ARRAY_CLOSE_BRACKET)
	{
	    return 1;
	}

	if (token.Type == JSON_TOKEN_TYPE_COMMA)
	    continue;

	if (token.Type != type || index >= count)
	    return 0;
    }
}

force_inline JsonValue
_json_create_array(JsonToken* tokens, JsonTokenType type, i32* index)
{
    JsonValue result;

    switch (type)
    {
    case JSON_TOKEN_TYPE_STRING:
	result.Type = JSON_VALUE_TYPE_STRING_ARRAY;
	break;
    case JSON_TOKEN_TYPE_INT_NUMBER:
	result.Type = JSON_VALUE_TYPE_INT;
	break;
    case JSON_TOKEN_TYPE_FLOAT_NUMBER:
	result.Type = JSON_VALUE_TYPE_FLOAT;
	break;
    }

    void* data = NULL;
    i32 i = *index;

    while (1)
    {
	JsonToken token = tokens[i];

	switch (token.Type)
	{
	case JSON_TOKEN_TYPE_STRING:
	{
	    char** strData = (char**) data;
	    result.Type = JSON_VALUE_TYPE_STRING_ARRAY;
	    array_push(strData, ((char*)(token.Data)));
	    data = strData;
	    break;
	}
	case JSON_TOKEN_TYPE_INT_NUMBER:
	{
	    i32* iData = (i32*) data;
	    result.Type = JSON_VALUE_TYPE_I32_ARRAY;
	    array_push(iData, void_to_i32(token.Data));
	    data = iData;
	    break;
	}
	case JSON_TOKEN_TYPE_FLOAT_NUMBER:
	{
	    f32* fData = (f32*) data;
	    result.Type = JSON_VALUE_TYPE_F32_ARRAY;
	    array_push(fData, void_to_f32(token.Data));
	    data = fData;
	    break;
	}
	case JSON_TOKEN_TYPE_COMMA:
	    ++i;
	    continue;
	case JSON_TOKEN_TYPE_ARRAY_CLOSE_BRACKET:
	    goto endLabel;
	default:
	    assert(0 && "We don't support this type of arrays");
	    break;
	}

	++i;
    }

    //TODO (bies): delete this in the future
    assert(tokens[i].Type == JSON_TOKEN_TYPE_ARRAY_CLOSE_BRACKET);

endLabel:
    result.Data = data;
    *index = i;

    return result;
}

static JsonToken
_json_read_string_token(char* stream, i32* length)
{
    const i32 bufferSize = 256;
    char tempStringBuffer[bufferSize];
    i32 index = 0;

    memset(tempStringBuffer, '\0', bufferSize*sizeof(char));

    if (stream[1] == '\\')
    {
	JsonToken stringToken;
	stringToken.Type = JSON_TOKEN_TYPE_STRING;
	stringToken.Data = "";
	*length = 0;
	return stringToken;
    }

    /*

      we are here
      |
      v
      "string content"

    */
    ++stream;

    while (*stream != '"' && *stream != '\0')
    {
	tempStringBuffer[index] = *stream;
	++index;
	++stream;
    }

    *length = index + 1;

    /*

      we are here (we ++ in main loop)
      |
      v
      "string content"

    */

    JsonToken stringToken;
    stringToken.Type = JSON_TOKEN_TYPE_STRING;
    stringToken.Data = string(tempStringBuffer);

    return stringToken;
}

static JsonToken
_json_read_number_token(char* stream, i32* length, i32 negativePart)
{
    /*
      0123.456
      ^
    */
    i32 isIntPart = 1;
    i32 intIndex = 0;
    i32 floatIndex = 0;
    char c = *stream;
    const i32 intBufferLength = 128;
    const i32 floatBufferLength = 128;
    char intBuffer[intBufferLength];
    char floatBuffer[floatBufferLength];

    memset(intBuffer  , '\0', intBufferLength   * sizeof(char));
    memset(floatBuffer, '\0', floatBufferLength * sizeof(char));

    while ((c >= '0' && c <= '9') || c == '.')
    {
	switch (c)
	{
	case '0': case '1': case '2':
	case '3': case '4': case '5':
	case '6': case '7': case '8':
	case '9':
	{
	    if (isIntPart)
	    {
		intBuffer[intIndex] = c;
		++intIndex;
	    }
	    else
	    {
		floatBuffer[floatIndex] = c;
		++floatIndex;
	    }
	    break;
	}

	case '.':
	{
	    if (!isIntPart)
	    {
		assert(0 && "Two dots in number are not acceptable!");
	    }

	    floatBuffer[floatIndex] = '0';
	    ++floatIndex;
	    floatBuffer[floatIndex] = '.';
	    ++floatIndex;

	    isIntPart = 0;

	    break;
	}
	}

	++stream;
	c = *stream;
    }

    *length = intIndex - 1;

    f32 result = string_to_i32(intBuffer);

    if (!isIntPart)
    {
	f32 floatPart = string_to_f32(floatBuffer);
	result += floatPart;
	*length += floatIndex - 1;
    }

    result *= negativePart;

    JsonToken token;
    if (!isIntPart)
    {
	token.Data = memory_allocate_type(f32);
	*((f32*)token.Data) = result;
	token.Type = JSON_TOKEN_TYPE_FLOAT_NUMBER;
    }
    else
    {
	token.Data = memory_allocate_type(i32);
	*((i32*)token.Data) = result;
	token.Type = JSON_TOKEN_TYPE_INT_NUMBER;
    }

    return token;
}

force_inline i32
_json_check_key_word(char* stream, const char* keyWord)
{
    i32 i;
    char* ptr = stream;
    i32 keyWordLength = istring_length(keyWord);

    for (i = 0; i < keyWordLength; ++i)
    {
	if (*ptr != keyWord[i])
	{
	    return 0;
	}

	++ptr;
    }

    return 1;
}

static i32
_json_next_bool(char* stream)
{
    char* ptr = stream;
    ++ptr;

    if (*ptr != 'r') return 0;
    if (*ptr != 'u') return 0;
    if (*ptr != 'e') return 0;

    return 1;
}

static JsonToken*
_json_read_tokens(JsonParser* parser, char* stream)
{
    char c;
    JsonToken* tokens = NULL;

    while ((c = *stream) != '\0')
    {
	_skip_spaces(stream);

	switch (c)
	{
	case '{':
	{
	    JsonToken token;
	    token.Type = JSON_TOKEN_TYPE_OBJECT_OPEN_BRACKET;
	    token.Data = "{";
	    array_push(tokens, token);
	    break;
	}

	case '}':
	{
	    JsonToken token;
	    token.Type = JSON_TOKEN_TYPE_OBJECT_CLOSE_BRACKET;
	    token.Data = "}";
	    array_push(tokens, token);
	    break;
	}

	case ',':
	{
	    JsonToken token;
	    token.Type = JSON_TOKEN_TYPE_COMMA;
	    token.Data = ",";
	    array_push(tokens, token);
	    break;
	}

	case '"':
	{
	    i32 length;
	    JsonToken stringToken = _json_read_string_token(stream, &length);
	    array_push(tokens, stringToken);

	    stream += length;

	    break;
	}

	case ':':
	{
	    JsonToken keyValueToken;
	    keyValueToken.Type = JSON_TOKEN_TYPE_KEYVALUE_SEPARATOR;
	    keyValueToken.Data = ":";
	    array_push(tokens, keyValueToken);
	    break;
	}

	case 't':
	{
	    if (_json_check_key_word(stream, TrueKeyWord))
	    {
		JsonToken trueValueToken;
		trueValueToken.Type = JSON_TOKEN_TYPE_BOOL_TRUE;
		trueValueToken.Data = (void*)TrueKeyWord;
		stream += 2;
		array_push(tokens, trueValueToken);
	    }
	    break;
	}
	case 'f':
	{
	    if (_json_check_key_word(stream, FalseKeyWord))
	    {
		JsonToken falseValueToken;
		falseValueToken.Type = JSON_TOKEN_TYPE_BOOL_FALSE;
		falseValueToken.Data = (void*)FalseKeyWord;
		stream += 3;
		array_push(tokens, falseValueToken);
	    }
	    break;
	}
	case 'n':
	{
	    if (_json_check_key_word(stream, NullKeyWord))
	    {
		JsonToken token;
		token.Type = JSON_TOKEN_TYPE_NULL;
		token.Data = (void*)NullKeyWord;
		stream += 3;
		array_push(tokens, token);
	    }
	    break;
	}

	case '[':
	{
	    JsonToken token;
	    token.Type = JSON_TOKEN_TYPE_ARRAY_OPEN_BRACKET;
	    token.Data = "[";
	    array_push(tokens, token);
	    break;
	}

	case ']':
	{
	    JsonToken token;
	    token.Type = JSON_TOKEN_TYPE_ARRAY_CLOSE_BRACKET;
	    token.Data = "]";
	    array_push(tokens, token);
	    break;
	}

	case '0': case '1': case '2':
	case '3': case '4': case '5':
	case '6': case '7': case '8':
	case '9':
	{
	    i32 length, negativePart = (*(stream - 1) == '-') ? -1 : 1;

	    JsonToken token = _json_read_number_token(stream, &length, negativePart);
	    array_push(tokens, token);
	    stream += length;

	    break;
	}

	}

	++stream;
    }

    return tokens;
}

static JsonValue _json_parse_value(JsonToken* tokens, i32* index);

static JsonObject*
json_parser_create_json_object(JsonToken* tokens, i32* index)
{
    i32 i = *index,
	isAfterKey = 0,
	count = array_count(tokens);
    JsonToken token;
    JsonObject* object = memory_allocate_type(JsonObject);
    object->Keys = NULL;
    object->Values = NULL;

    while (1)
    {
	token = tokens[i];

	if (token.Type == JSON_TOKEN_TYPE_OBJECT_CLOSE_BRACKET || i >= count)
	    break;

	if (!isAfterKey && token.Type == JSON_TOKEN_TYPE_STRING)
	{
	    array_push(object->Keys, token.Data);
	    isAfterKey = 1;
	}
	else if (isAfterKey && token.Type != JSON_TOKEN_TYPE_KEYVALUE_SEPARATOR && token.Type != JSON_TOKEN_TYPE_COMMA)
	{
	    // json value can contains a json object
	    JsonValue value = _json_parse_value(tokens, &i);
	    array_push(object->Values, value);
	    isAfterKey = 0;
	}

	++i;
    }

    *index = i;

    return object;
}

static JsonValue
_json_parse_value(JsonToken* tokens, i32* index)
{
    JsonValue result;
    i32 i = *index;
    JsonToken token = tokens[i];

    switch (token.Type)
    {
    case JSON_TOKEN_TYPE_NULL:
    {
	result.Type = JSON_VALUE_TYPE_NULL;
	result.Data = token.Data;
	break;
    }

    case JSON_TOKEN_TYPE_STRING:
    {
	result.Type = JSON_VALUE_TYPE_STRING;
	result.Data = token.Data;
	break;
    }

    case JSON_TOKEN_TYPE_INT_NUMBER:
    {
	result.Type = JSON_VALUE_TYPE_INT;
	result.Data = token.Data;
	break;
    }

    case JSON_TOKEN_TYPE_FLOAT_NUMBER:
    {
	result.Type = JSON_VALUE_TYPE_FLOAT;
	result.Data = token.Data;
	break;
    }

    case JSON_TOKEN_TYPE_BOOL_TRUE:
    case JSON_TOKEN_TYPE_BOOL_FALSE:
    {
	result.Type = JSON_VALUE_TYPE_BOOL;
	result.Data = token.Data;
	break;
    }

    case JSON_TOKEN_TYPE_ARRAY_OPEN_BRACKET:
    {
	++i;

	if (_json_is_array_of_type(tokens, JSON_TOKEN_TYPE_STRING, i))
	{
	    result = _json_create_array(tokens, JSON_TOKEN_TYPE_STRING, &i);
	}
	else if (_json_is_array_of_type(tokens, JSON_TOKEN_TYPE_INT_NUMBER, i))
	{
	    result = _json_create_array(tokens, JSON_TOKEN_TYPE_INT_NUMBER, &i);
	}
	else if (_json_is_array_of_type(tokens, JSON_TOKEN_TYPE_FLOAT_NUMBER, i))
	{
	    result = _json_create_array(tokens, JSON_TOKEN_TYPE_FLOAT_NUMBER, &i);
	}
	else
	{
	    assert(0 && "Not supported array!");
	}

	break;
    }

    case JSON_TOKEN_TYPE_OBJECT_OPEN_BRACKET:
    {
	result.Type = JSON_VALUE_TYPE_OBJECT;
	result.Data = json_parser_create_json_object(tokens, &i);
	assert(tokens[i].Type == JSON_TOKEN_TYPE_OBJECT_CLOSE_BRACKET);
	break;
    }

    }

    *index = i;

    return result;
}

static void
json_parse_string(JsonParser* parser, char* string)
{
    _register_keywords();

    assert(string != NULL && "JsonParser can't parse NULL string!");

    i32 index = 0;
    parser->SourceContent = string;
    parser->Tokens = _json_read_tokens(parser, string);
    parser->Object = json_parser_create_json_object(parser->Tokens, &index);
}

static void
json_parse_file(JsonParser* parser, const char* path)
{
    char* data = file_read_string(path);
    json_parse_string(parser, data);
}

static void
json_destroy(JsonParser* parser)
{
    JsonObject* obj = parser->Object;

    json_object_destroy(obj);

    memory_free(parser->SourceContent);
    memory_free(parser->Tokens);

    parser->SourceContent = NULL;
    parser->Object = NULL;
    parser->Tokens = NULL;
}

static void
json_write_file(JsonParser* parser, const char* path)
{
    char* sb = NULL;
    i32 i, count = array_count(parser->Tokens),
	isInArray = 0;

    char intBuffer[32];
    char floatBuffer[32];
    i32 indentationLevel = 0;
    const char* tab4 = "    ";

    for (i = 0; i < count; ++i)
    {
	JsonToken token = parser->Tokens[i];

	if (token.Type == JSON_TOKEN_TYPE_OBJECT_OPEN_BRACKET)
	{
	    ++indentationLevel;
	}
	else if (token.Type == JSON_TOKEN_TYPE_OBJECT_CLOSE_BRACKET)
	{
	    --indentationLevel;
	}

	if (token.Type == JSON_TOKEN_TYPE_ARRAY_OPEN_BRACKET)
	    isInArray = 1;
	else if (token.Type == JSON_TOKEN_TYPE_ARRAY_CLOSE_BRACKET)
	    isInArray = 0;

	switch (token.Type)
	{
	case JSON_TOKEN_TYPE_INT_NUMBER:
	{
	    memset(intBuffer, '\0', sizeof(intBuffer));
	    i32 number = void_to_i32(token.Data);
	    string_i32(intBuffer, number);
	    string_builder_appends(sb, intBuffer);
	    break;
	}

	case JSON_TOKEN_TYPE_FLOAT_NUMBER:
	{
	    memset(floatBuffer, '\0', sizeof(floatBuffer));
	    f32 number = void_to_f32(token.Data);
	    string_f64(floatBuffer, number);
	    string_builder_appends(sb, floatBuffer);
	    break;
	}

	case JSON_TOKEN_TYPE_KEYVALUE_SEPARATOR:
	{
	    string_builder_appends(sb, token.Data);
	    string_builder_appends(sb, " ");
	    break;
	}

	case JSON_TOKEN_TYPE_STRING:
	{
	    string_builder_appends(sb, "\"");
	    string_builder_appends(sb, token.Data);
	    string_builder_appends(sb, "\"");

	    break;
	}

	default:
	{
	    string_builder_appends(sb, token.Data);
	    break;
	}
	}

	if ((token.Type == JSON_TOKEN_TYPE_COMMA
	     || token.Type == JSON_TOKEN_TYPE_OBJECT_OPEN_BRACKET)
	    && !isInArray)
	{
	    string_builder_appends(sb, "\n");
	    for (i32 i = 0; i < indentationLevel; i++)
		string_builder_appends(sb, tab4);
	}

	if (parser->Tokens[i + 1].Type == JSON_TOKEN_TYPE_OBJECT_OPEN_BRACKET)
	{
	    string_builder_appends(sb, "\n");
	    for (i32 i = 0; i < indentationLevel; i++)
		string_builder_appends(sb, tab4);
	}
	else if (parser->Tokens[i + 1].Type == JSON_TOKEN_TYPE_OBJECT_CLOSE_BRACKET)
	{
	    string_builder_appends(sb, "\n");
	    for (i32 i = 0; i < (indentationLevel - 1); i++)
		string_builder_appends(sb, tab4);
	}
    }

    file_write_string(path, sb, string_builder_count(sb));
}

static JsonValue
json_object_get_value(JsonObject* object, char* key, JsonValueType type)
{
    const char** keys = object->Keys;
    i32 i, count = array_count(keys);
    for (i = 0; i < count; ++i)
    {
	if (string_compare(keys[i], key))
	{
	    return object->Values[i];
	}
    }

    assert(0 && "Expected key that exist in current object!");
    return (JsonValue) {};
}

#define ind(sb, indentation)			\
    ({						\
	const char* tab4 = "    ";		\
	for (i32 i = 0; i < indentation; ++i)	\
	{					\
	    string_builder_appends(sb, tab4);	\
	}					\
    })

static char*
json_object_to_string(JsonObject* root, i32 startIndentationLevel)
{
    char** keys = (char**)root->Keys;
    JsonValue* values = (JsonValue*)root->Values;

    assert(keys != NULL && "Keys is NULL wtf");
    assert(values != NULL && "Values is NULL wtf");

    char* sb = NULL;
    i32 indentationLevel = startIndentationLevel;

    ind(sb, indentationLevel);

    string_builder_appends(sb, "{\n");
    ++indentationLevel;

    i32 keysCount = array_count(keys);
    for (i32 i = 0; i < keysCount; ++i)
    {
	char* key = keys[i];
	JsonValue value = values[i];

	ind(sb, indentationLevel);

	string_builder_appendf(sb, "\"%s\": ", key);

	switch (value.Type)
	{
	case JSON_VALUE_TYPE_INT:
	{
	    string_builder_appendf(sb, "%d", void_to_i32(value.Data));
	    break;
	}
	case JSON_VALUE_TYPE_FLOAT:
	{
	    string_builder_appendf(sb, "%f", void_to_f32(value.Data));
	    break;
	}
	case JSON_VALUE_TYPE_NULL:
	case JSON_VALUE_TYPE_BOOL:
	{
	    string_builder_appendf(sb, "%s", void_to_string(value.Data));
	    break;
	}
	case JSON_VALUE_TYPE_STRING:
	{
	    string_builder_appendf(sb, "\"%s\"", void_to_string(value.Data));
	    break;
	}
	case JSON_VALUE_TYPE_I32_ARRAY:
	{
	    i32* array = (i32*)value.Data;
	    i32 count = array_count(array);
	    string_builder_appendc(sb, '[');
	    for (i32 i = 0; i < count; ++i)
	    {
		if (i < (count - 1))
		{
		    string_builder_appendf(sb, "%d, ", array[i]);
		}
		else
		{
		    string_builder_appendf(sb, "%d", array[i]);
		}
	    }
	    string_builder_appendc(sb, ']');
	    break;
	}
	case JSON_VALUE_TYPE_F32_ARRAY:
	{
	    f32* array = (f32*)value.Data;
	    i32 count = array_count(array);
	    string_builder_appendc(sb, '[');
	    for (i32 i = 0; i < count; ++i)
	    {
		if (i < (count - 1))
		{
		    string_builder_appendf(sb, "%f, ", array[i]);
		}
		else
		{
		    string_builder_appendf(sb, "%f", array[i]);
		}
	    }
	    string_builder_appendc(sb, ']');
	    break;
	}
	case JSON_VALUE_TYPE_STRING_ARRAY:
	{
	    char** array = (char**)value.Data;
	    i32 count = array_count(array);
	    string_builder_appendc(sb, '[');
	    for (i32 i = 0; i < count; ++i)
	    {
		if (i < (count - 1))
		{
		    string_builder_appendf(sb, "\"%s\", ", array[i]);
		}
		else
		{
		    string_builder_appendf(sb, "\"%s\"", array[i]);
		}
	    }
	    string_builder_appendc(sb, ']');

	    break;
	}
	case JSON_VALUE_TYPE_OBJECT:
	{
	    JsonObject* obj = (JsonObject*) value.Data;
	    if (obj->Keys != NULL && obj->Values != NULL)
	    {
		char* objToStr = json_object_to_string(obj, indentationLevel);
		string_builder_appendf(sb, "\n%s", objToStr);
	    }
	    else
	    {
		string_builder_appends(sb, " \"NULL\"");
	    }

	    break;
	}

	}

	if (i < (keysCount - 1))
	{
	    string_builder_appends(sb, ",\n");
	}
	else
	{
	    string_builder_appends(sb, "\n");
	}
    }

    ind(sb, indentationLevel - 1);
    string_builder_appendc(sb, '}');

    return sb;
}

/*
  PUBLIC JSON UTILS
*/

static const char* g_TokensTypeToString[] = {
    "JSON_TOKEN_TYPE_NONE",
    "JSON_TOKEN_TYPE_KEYVALUE_SEPARATOR",
    "JSON_TOKEN_TYPE_STRING",
    "JSON_TOKEN_TYPE_INT_NUMBER",
    "JSON_TOKEN_TYPE_FLOAT_NUMBER",
    "JSON_TOKEN_TYPE_BOOL_TRUE",
    "JSON_TOKEN_TYPE_BOOL_FALSE",
    "JSON_TOKEN_TYPE_NULL",
    "JSON_TOKEN_TYPE_OBJECT_OPEN_BRACKET",
    "JSON_TOKEN_TYPE_OBJECT_CLOSE_BRACKET",
    "JSON_TOKEN_TYPE_ARRAY_OPEN_BRACKET",
    "JSON_TOKEN_TYPE_ARRAY_CLOSE_BRACKET",
    "JSON_TOKEN_TYPE_COMMA"
};

static const char*
json_token_to_string(JsonToken token)
{
    return g_TokensTypeToString[token.Type];
}

static char*
json_tokens_to_string(JsonToken* tokens)
{
    i32 i;
    i32 count = array_count(tokens);
    char* sb = NULL;
    i32 indentation = 0;

    for (i = 0; i < count; ++i)
    {
	JsonToken token = tokens[i];

	switch (token.Type)
	{
	case JSON_TOKEN_TYPE_OBJECT_OPEN_BRACKET:
	{
	    if (i != 0)
		string_builder_appends(sb, "\n");
	    ind(sb, indentation);
	    string_builder_appends(sb, "{\n");
	    ++indentation;
	    break;
	}
	case JSON_TOKEN_TYPE_OBJECT_CLOSE_BRACKET:
	{
	    --indentation;
	    ind(sb, indentation);
	    string_builder_appends(sb, "}");
	    break;
	}
	case JSON_TOKEN_TYPE_ARRAY_OPEN_BRACKET:
	{
	    string_builder_appends(sb, "[ ");
	    ++i;
	    while (1)
	    {
		token = tokens[i];

		if (token.Type == JSON_TOKEN_TYPE_ARRAY_CLOSE_BRACKET)
		{
		    break;
		}

		if (token.Type == JSON_TOKEN_TYPE_COMMA)
		{
		    ++i;
		    continue;
		}

		const char* tokenStr = NULL;// = json_token_to_string(token);

		switch (token.Type)
		{
		case JSON_TOKEN_TYPE_STRING:
		{
		    tokenStr = token.Data;
		    break;
		}
		case JSON_TOKEN_TYPE_INT_NUMBER:
		{
		    tokenStr = "I32";
		    break;
		}
		case JSON_TOKEN_TYPE_FLOAT_NUMBER:
		{
		    tokenStr = "F32";
		    break;
		}
		}

		if (tokens[i + 1].Type != JSON_TOKEN_TYPE_ARRAY_CLOSE_BRACKET)
		{
		    string_builder_appendf(sb, "%s, ", tokenStr);
		}
		else
		{
		    string_builder_appendf(sb, "%s", tokenStr);
		}

		++i;
	    }

	    string_builder_appends(sb, " ]");

	    break;
	}
	case JSON_TOKEN_TYPE_COMMA:
	{
	    string_builder_appends(sb, ",\n");
	    break;
	}
	case JSON_TOKEN_TYPE_KEYVALUE_SEPARATOR:
	{
	    string_builder_appends(sb, ": ");
	    break;
	}
	case JSON_TOKEN_TYPE_STRING:
	{
	    if (tokens[i - 1].Type != JSON_TOKEN_TYPE_KEYVALUE_SEPARATOR)
		ind(sb, indentation);
	    string_builder_appendf(sb, "\"%s\"", token.Data);
	    break;
	}
	case JSON_TOKEN_TYPE_INT_NUMBER:
	{
	    string_builder_appends(sb, "I32");
	    break;
	}
	case JSON_TOKEN_TYPE_FLOAT_NUMBER:
	{
	    string_builder_appends(sb, "F32");
	    break;
	}
	default:
	{
	    const char* tokenStr = json_token_to_string(token);
	    string_builder_appendf(sb, "%s", tokenStr);
	    break;
	}
	}

	if (tokens[i + 1].Type == JSON_TOKEN_TYPE_OBJECT_CLOSE_BRACKET)
	{
	    string_builder_appends(sb, "\n");
	}
    }

    return sb;
}

static void
json_object_free(JsonObject* obj)
{
    i32 i, count = array_count(obj->Values);
    for (i = 0; i < count; ++i)
    {
	JsonValue value = obj->Values[i];
	if (value.Type == JSON_VALUE_TYPE_OBJECT)
	{
	    json_object_free((JsonObject*)value.Data);
	}
	else if (value.Type == JSON_VALUE_TYPE_I32_ARRAY
		 || value.Type == JSON_VALUE_TYPE_F32_ARRAY
		 || value.Type == JSON_VALUE_TYPE_STRING_ARRAY)
	{
	    array_free(value.Data);
	}
    }

    array_free(obj->Keys);
    array_free(obj->Values);

    memory_free(obj);
}

static void
_json_print_value(JsonValue value)
{
    switch (value.Type)
    {
    case JSON_VALUE_TYPE_INT:
    {
	GERROR("JsonValue: %d\n", void_to_i32(value.Data));
	break;
    }

    case JSON_VALUE_TYPE_FLOAT:
    {
	GERROR("JsonValue: %f\n", void_to_f32(value.Data));
	break;
    }

    case JSON_VALUE_TYPE_NULL:
    {
	GERROR("JsonValue: NULL\n");
	break;
    }

    case JSON_VALUE_TYPE_BOOL:
    case JSON_VALUE_TYPE_STRING:
    {
	GERROR("JsonValue: %s\n", void_to_string(value.Data));
	break;
    }

    case JSON_VALUE_TYPE_I32_ARRAY:
    case JSON_VALUE_TYPE_F32_ARRAY:
    case JSON_VALUE_TYPE_STRING_ARRAY:
    {
	GERROR("JsonValue: THIS IS ARRAY\n");
	break;
    }

    case JSON_VALUE_TYPE_OBJECT:
    {
	GERROR("JsonValue: THIS IS OBJECT\n");
	break;
    }
    }
}

#endif
