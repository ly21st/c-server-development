#include <string>
#include <jsoncpp/json/json.h>
#include <iostream>


/**
{
	"name":	"milo",
	"age":	80,
    "professional": {
        "english": 4,
        "putonghua": 2,
        "computer": 4
    },
    "languages":	["C++", "C"],
	"phone":	{
		"number":	"18620823143",
		"type":	"home"
	},
	"courses":	[{
			"name":	"Linux kernel development",
			"price":	"7.7"
		}, {
			"name":	"Linux server development",
			"price":	"8.0"
		}],
	"vip":	true,
	"address":	null
}
*/

void deserialization(std::string strValue);
std::string serialize();
 
int main(int argc, char** argv) {
	std::string out = serialize();
	deserialization(out);
	return 0;
}

//反序列化
void deserialization(std::string strValue) {
	using namespace std;

	Json::Reader reader;
	Json::Value json;
 
	if (!reader.parse(strValue, json)) {
		std::cout << "error" << std::endl;
		return;
	}

	std::string name = json["name"].asString();
	std::cout << name << std::endl;
	int age = json["age"].asInt();
	std::cout << age << std::endl;
	const Json::Value professional = json["professional"];
	std::string english = professional["english"].asString();
	std::string putonghua = professional["putonghua"].asString();
	std::string computer = professional["computer"].asString();
	std::cout << "english:" << english << std::endl;
	std::cout << "putonghua:" << putonghua << std::endl;
	std::cout << "computer:" << computer << std::endl;

	const Json::Value languages = json["languages"];
	for (unsigned int index = 0; index < languages.size(); ++index ) {
		std::string language = languages[index].asString();
		std::cout << language << std::endl;
	}

	const Json::Value phone = json["phone"];
	std::string number = phone["number"].asString();
	std::string type = phone["type"].asString();
	std::cout << "number:" << number << std::endl;
	std::cout << "type:" << type << std::endl;

	const Json::Value courses = json["courses"];
	for (unsigned int i = 0; i < courses.size(); i++) {
		std::string courseName = courses[i]["name"].asString();
		std::cout << "courseName:" << courseName << std::endl;
		double coursePrice = courses[i]["price"].asDouble();
		std::cout << "coursePrice:" << coursePrice << std::endl;
	}

	bool vip = json["vip"].asBool();
	if (vip) {
		std::cout << "true" << std::endl;
	} else {
		std::cout << "false" << std::endl;
	}

	if (json["address"].isNull()) {
        cout << "address is null!" << endl;
    } else {
		cout << "address not null!" << endl;
	}
}

//序列化
std::string serialize() {
	using namespace std;
 
	Json::Value root;

	Json::Value professional;
	Json::Value languages;
	Json::Value phone;
	Json::Value courses;
	Json::Value courseItem1;
	Json::Value courseItem2;

	root["name"] = "milo";
	root["age"] = 80;
	professional["english"] = 4;
	professional["putonghua"] = 2;
	professional["computer"] = 4;

	languages.append("C++");
	languages.append("C");

	phone["number"] = "18620823143";
	phone["type"] = "home";
 
	root["professional"] = professional;
	root["languages"] = languages;
	root["phone"] = phone;

	courseItem1["name"]="Linux kernel developmen";
	courseItem1["price"]=7;
	courseItem2["name"]="Linux server developmen";
	courseItem2["price"]=8;

	courses.append(courseItem1);
	courses.append(courseItem2);

	root["courses"] = courses;

	root["vip"] = true;
	root["address"] = Json::nullValue;
 
	root.toStyledString();
	std::string out = root.toStyledString();
	std::cout << out << std::endl;
	return out;
}