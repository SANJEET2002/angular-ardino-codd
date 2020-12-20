#ifndef JSON_H__
#define JSON_H__
  #ifndef PRINT
  #define PRINT
    #define lineBreak Serial.println()
    #define var(x) #x
    #define Print(x) Serial.println(String(var(x)) + " : " + (x))
  #endif
  class JSON {
    String jsonString;
    String readFromIndex(int);
    int totalLength;
    public:
      JSON() {
        jsonString = "{}";
        updateLength();
      }
      JSON(String json) {
        if (!json.length()) {
          json = "{}";
        }
        assign(json);
        updateLength();
      }
      JSON(char* js) {
        if (!strlen(js)) {
          assign("{}");
        } else {
          assign(String(js));
        }
        updateLength();
      }

      template<typename T, typename U>
      String keyValuePair(T key, U value) {
        JSON js;
        js.assign(value);
        return String('"') + key + "\":\"" + js.toString() + '"';
      }
      template<typename T>
      JSON& assign(T );
      template <typename T, typename U>
      JSON& set(T key, U value) {
        if (isJSON()) {
          if (has(key)) {
            jsonString.replace(keyValuePair(key, get(key)), keyValuePair(key, value));
          } else {
            jsonString += "}}";
            jsonString.replace("}}}", String(",") + keyValuePair(key, value) + "}");
            jsonString.replace("{,", "{");
          }
        } else if (isArray() && validIndex(key)) {
          int length = jsonString.length();
          int startIndex = 2;
          int keyEx = String(key).toInt();
          
          while (startIndex < length) {
            if (keyEx == 0) {
              int end = endIndexOf(startIndex);
              jsonString = jsonString.substring(0, startIndex) + value + jsonString.substring(end);
              updateLength();
              return *this;
//              return jsonString.substring(startIndex, endIndexOf(startIndex));
            }
            keyEx--;
            startIndex = endIndexOf(startIndex) + 3;
          }
        }
        updateLength();
        return *this;
      }
      template <typename T>
      JSON push(T val) {
        JSON value;
        value.assign(val);
        if (isArray()) {
          if (jsonString == "[]") {
            jsonString =  String("[\"") + value.toString() + "\"]";
          } else {
            jsonString += "~~";
            jsonString.replace("]~~", String(",\"") + value.toString() + "\"]");
          }
        }
        updateLength();
        return *this;
      }
            
      template <typename T>
      String get(T );
      
      template <typename T>
      JSON& remove(T key) {
        if (has(key)) {
          jsonString.replace(keyValuePair(key, get(key)), "");
          jsonString.replace("\",,\"", "\",\"");
          jsonString.replace("{,", "{");
          jsonString.replace(",}", "}");
        }
        return *this;
      }
      
      template <typename T>
      String getKey(T key) {
        return String('"') + key + "\":\"";
      }

      template <typename T>
      bool has(T );

      char* stringify() {
        jsonString.replace("\"{", "{");
        jsonString.replace("\"[", "[");
        jsonString.replace("}\"", "}");
        jsonString.replace("]\"", "]");
        return strdup(jsonString.c_str());
      }
      template <typename T>
      JSON operator [](T key) {
        return get(key);
      }
      
      int endIndexOf(int);
      int toInt() {
        return jsonString.toInt();
      }
      float toFloat() {
        return jsonString.toFloat();
      }
      String toString() {
        if (jsonString == "{}") {
          return "";
        }
        return jsonString;
      }

      bool isJSON() {
        return jsonString[0] == '{' && jsonString[jsonString.length() - 1] == '}';
      }

      bool isArray() {
        return jsonString[0] == '[' && jsonString[jsonString.length() - 1] == ']';
      }

      void reset() {
        jsonString = "{}";
        updateLength();
      }
  
      JSON& print() {
        Serial.println(jsonString);
        return *this;
      }
      
      void serialize() {
        print();
    }
    bool validIndex(int key) {
      return true;
    }
    bool validIndex(String key) {
      return false;
    }
    bool validIndex(char key) {
      return false;
    }
    void show();
    inline void printTab(int, int);
    void updateLength() { 
      totalLength = 0;     
      
      if (isArray()) {
        int i = 0;
        while(1) {
          if (this->get(i++).length()) {
             totalLength++;
          } else {
            break;
          }
        }
      } 
    }
    int length() {
      return totalLength;
    }
  };

  template<typename T>
  JSON& JSON :: assign(T js) {
    String json(js);
    
    json.replace("  ", " ");
    json.replace(" :", ":");
    json.replace(": ", ":");
    json.replace(":", "\":\"");
    json.replace(", ", ",");
    json.replace(" ,", ",");
    json.replace(",", "\",\"");
    json.replace("  ", " ");
    json.replace("\\\"", "\"");
    json.replace("{ ", "{");
    json.replace(" }", "}");
    json.replace("}", "\"}");
    json.replace("{", "{\"");
    
    json.replace("[ ", "[");
    json.replace(" ]", "]");
    json.replace("]", "\"]");
    json.replace("[", "[\"");
    
    json.replace("\"\"", "\"");
    json.replace(":\",", ":\"\",");
    
    json.replace(":\"}", ":\"\"}");
    json.replace("{\"}", "{}");
    json.replace("{\"\"}", "{}");
    
    json.replace(":\"]", ":\"\"]");
    json.replace("[\"]", "[]");
    json.replace("[\"\"]", "[]");
    
    json.replace("\",,\"", "\",\"");
    json.replace("{,", "{");
    json.replace(",}", "}");
    
    json.replace("\",,\"", "\",\"");
    json.replace("[,", "[");
    json.replace(",]", "]");
    
    json.trim();
    
    jsonString = json;
  }
  
  template <typename T>
  String JSON :: get(T key) {
    if (has(key)) {
      int startIndex = jsonString.indexOf(getKey(key)) + getKey(key).length();
      return jsonString.substring(startIndex, endIndexOf(startIndex));
    } else if (validIndex(key) && isArray()) {
      int length = jsonString.length();
      int startIndex = 2;
      int keyEx = String(key).toInt();
      while (startIndex < length) {
        if (jsonString[startIndex - 1] == '{') {
          startIndex --;
        }
        if (keyEx == 0) {
          return jsonString.substring(startIndex, endIndexOf(startIndex));
        }
        keyEx--;
        startIndex = endIndexOf(startIndex) + 3;
      }
      return jsonString.substring(startIndex, endIndexOf(startIndex));
    } else {
      return String();
    }
  }
  
  template <typename T>
  bool JSON :: has(T key) {
    int index = jsonString.indexOf(getKey(key));
    if (index < 0) {
      return false;
    }
    int depth = 0;
    while (index) {
      switch(jsonString[index]) {
        case '{': depth ++; break;
        case '[': depth ++; break;
        case ']': depth --; break;
        case '}': depth --; break;
      }
      index --;
    }
    return depth == 0;        
  }

  int JSON :: endIndexOf(int currentIndex) {
    int braces = 0;
    int squareBracket = 0;
    int index = currentIndex;
    char ch;
    while((ch = jsonString[index]) != NULL) {
      if (ch == '{') {
        braces ++;
      }
      if (ch == '[') {
        squareBracket ++;
      }
      if (ch == '}') {
        braces --;
      }
      if (ch == ']') {
        squareBracket --;
      }
      if (braces == 0 && squareBracket == 0 && ch == '"') {
        break;
      }
      index++;
    }
    return index;
  }

  inline void JSON :: printTab(int depth, int width = 3) {
    Serial.println();
    for(int i=0; i<depth*width; i++) {
      Serial.print(' ');
    }
  }
  void JSON :: show() {
    int depth = 0;
    int tabWidth = 3;
    String displayString = stringify();
    
    int length = displayString.length();
    for (int i=0; i<length; i++) {
      char ch = displayString[i];
      Serial.print(ch);
      if (ch == ':') {
        Serial.print(' ');
      }
      if (ch == '{' || ch == '[') {
        depth++;
        printTab(depth);
      }
      if (ch == '}' || ch == ']') {
        if (displayString[i+1] == ',') {
          Serial.print(',');
          i++;
          depth++;
        }
        depth --;
        printTab(depth);
      }
      if (ch == '"' && (displayString[i+1] == ']' || displayString[i+1] == '}')) {
        depth --;
        printTab(depth); 
      }
      if (ch == ',') {
        printTab(depth);
      }
    }
  }
#endif
