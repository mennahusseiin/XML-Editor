#include <iostream>
#include <string>
#include <stack>

using namespace System;
using namespace std;

string RemoveSpaces(string xml) {
    string removed = "";
    string s, s1;
    int size = xml.length();
    int i = 0;

    while (i < size) {
        s = xml[i];
        //if (i < size - 1) {
            s1 = xml[i + 1];

            if (s == " " && s1 == " ") {
                i += 2;
            }

            else if (s == "\r" || s == "\n" || s == "\t") {
                i++;
            }
            
            else {
                removed += s;
                i++;
            }
        //}

        /*else if (i == (size - 1)) {
            if (s == "\r" || s == "\n" || s == "\t" || s == " ") {
                break;
            }

            else {
                removed += s;
            }
        }*/

    }
    return removed;
}

String^ toJson(string s)
{
    int size = s.length();
    string json = "{";
    string append = "*";
    string w, word = "";

    //i for xml length, j for json length, x for repeated tags
    int i = 0, j = 1, x=0;

    //stk for opening tags, tag for opening tags but they are poped when they are closed, brackets for({,]) 
    stack<string> stk, tag, brackets;

    //it's true if opening tag,and false if closing
    stack<bool> opening;

    //tags holds last closed tag, bracket holds last printed bracket
    string tags, bracket;

    //saves value of x
    stack<int> ind;
    while (i < size) {
        w = s[i];
        
        if (w == "\r" || w == "\n") {

        }

        //checking if it's tag or data to be printed
        if (w == "<") {
            i++;
            w = s[i];
            if (w != "/") { //if opening tag
                while (w != ">") {
                    word += w;
                    i++;
                    w = s[i];
                }
                //if more than one <topic> repeated in scope of <topics>, delete topic from json
                if (word == tags) {
                    json.erase(x - ((brackets.size() + 1)) - 2, word.length() + 6 + ((brackets.size() + 1))); //6 bec ({ \n " " : ), x->"
                    j -= (word.length() + 6 + ((brackets.size() + 1)));
                    json.insert(x - ((brackets.size() + 1)) - 2, "[\n");
                    j += 2;
                    brackets.pop();
                    brackets.push("]");
                    //return old value of x
                    if (!ind.empty()) {
                        ind.pop();
                        if (!ind.empty()) {
                            x = ind.top();
                        }
                    }
                    //skip new <topic>
                    i++;
                    stk.push(word);
                    tag.push(word);
                    opening.push(true);
                    json += "\n";
                    j += 1;
                    word = "";
                    continue;
                }
                //condition for the first opening tag bec stk is still empty
                if (stk.empty()) {
                    json += "\n";
                }
                //if last thing in json was closing tag, no need to open "{"
                else if (opening.top() == false) {

                    json += "\n";
                    j += 1;
                }
                else {
                    //for formating
                    for (int t = brackets.size() + 1; t > 0; t--) {
                        json += " ";
                        j += 1;
                    }
                    json += "{\n";
                    j += 2;

                    //assuming if there is array in xml file, it must be writtin as (users,user - topics,topic)
                    //save index of user in json to be deleted if it's repeated in the same scope of users
                    if (word + "s" == stk.top()) {
                        x = j + brackets.size() + 3; 

                        ind.push(x);
                    }
                    brackets.push("}");
                }

                //for formating
                for (int t = brackets.size() + 1; t > 0; t--) {
                    json += " ";
                    j += 1;
                }
                json += "\"";
                j++;
                json += word;
                j += word.length();
                json += "\": ";
                j += 3;
                stk.push(word);
                tag.push(word);
                i++;
                opening.push(true);
            }
            else {  //if closing tag
                while (w != ">") { //don't print closing tags
                    i++;
                    w = s[i];
                    word += w;
                }
                word.pop_back();

                //opening tags are pushed into tag*stack* and poped when they are closed
                //before poping they are stored in tags*string*
                //if opening tag is repeated after the closing tag, it will be deleted from json*string*
                if (word == tag.top()) {
                    tags = word;
                    tag.pop();

                    //"topic", x=90->", topics length = count = 6, 
                    int count = tags.length();
                    string currentWord;
                    currentWord = json[x + 1];
                    //currentWord = t

                    //if scope of topics ended without repeating topic, remove index of topic.
                    for (int L = x + 2; L < x + count; L++) {
                        currentWord += json[L]; //topic
                    }

                    //topic + s == topics
                    if (currentWord + "s" == tags) {
                        //pop 90
                        ind.pop();
                        //x = index of post
                        if (!ind.empty()) {
                            x = ind.top();
                        }
                    }
                }
                //print closing brackets
                if (opening.top() == false) {
                    string back;
                    back = json.back();

                    //removing unwanted commas
                    if ((brackets.top() == "]" && bracket == "}") || (brackets.top() == "}" && back == ",") || (brackets.top() == "]" && back == ","))
                    { //brackets.top()-> barcket to be printed, bracket->last printed bracket
                        json.pop_back();
                        j--;
                    }
                    json += "\n";
                    j+= 1;
                    for (int t = brackets.size(); t > 0; t--) {
                        json += " ";
                        j += 1;
                    }
                    json += brackets.top();
                    bracket = brackets.top();
                    j++;
                    brackets.pop();
                    if (!brackets.empty()) {
                        json += ","; j++;
                    }
                }
                i++;
                opening.push(false);
            }
        }
        //data to be printed charecter by charecter no need for word here
        else {
            json += "\"";
            j++;
            while (w != "<")
            {
                json += w;
                j++;
                i++;
                w = s[i];
            }
            json += "\"";
            json += ",";
            j += 2;
        }
        word = "";
    }
    json += "\n}";  //closing of json string
    j += 2;

    String^ JSON = gcnew String(json.c_str());
    return JSON;
}
