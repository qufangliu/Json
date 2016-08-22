#include "HelloWorldScene.h"

#include "json/rapidjson.h"
#include "json/document.h"
#include "json/filestream.h"
#include "json/stringbuffer.h"
#include "json/writer.h"

USING_NS_CC;

Scene* HelloWorld::createScene()
{
    auto scene = Scene::create();
    auto layer = HelloWorld::create();
    scene->addChild(layer);
    return scene;
}

bool HelloWorld::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    else {
        this->initSelf();
        return true;
    }
}

void HelloWorld::initSelf()
{
    //生成一串如下的json格式字符串，并解析
//    {
//        "name":"qfl",
//        "age":20,
//        "letter":["a","b","c"],
//        "location":{"province":"fujian","city":"xiamen","number":16}
//        "book":[{"name":"book1", "isbn":"123"},{"name":"book2","isbn":"456"}],
//        "healthy":true,
//        "sports":null,
//    }
    
    //生成Json串
    rapidjson::Document jsonDoc;    //生成一个dom元素
    rapidjson::Document::AllocatorType &allocator = jsonDoc.GetAllocator(); //获取分配器
    jsonDoc.SetObject();    //将当前的doc设置为一个object
    
    //添加属性
    jsonDoc.AddMember("name", "qfl", allocator);    //添加字符串值
    jsonDoc.AddMember("age", 20, allocator);        //添加int类型值
    
    //生成array
    rapidjson::Value letterArray(rapidjson::kArrayType);
    letterArray.PushBack("a", allocator);
    letterArray.PushBack("b", allocator);
    letterArray.PushBack("c", allocator);
    jsonDoc.AddMember("letter", letterArray, allocator);    //添加数组
    
    //生成一个object
    rapidjson::Value locationObj(rapidjson::kObjectType);
    locationObj.AddMember("province", "fujian", allocator);
    locationObj.AddMember("city", "xiamen", allocator);
    locationObj.AddMember("number", 16, allocator);
    jsonDoc.AddMember("location", locationObj, allocator);  //添加object
    
    //生成一个object数组
    rapidjson::Value bookArray(rapidjson::kArrayType);
    rapidjson::Value book1(rapidjson::kObjectType); //生成book1
    book1.AddMember("name", "book1", allocator);
    book1.AddMember("isbn", "123", allocator);
    bookArray.PushBack(book1, allocator);           //添加到数组
    rapidjson::Value book2(rapidjson::kObjectType); //生成book2
    book2.AddMember("name", "book2", allocator);
    book2.AddMember("isbn", "456", allocator);
    bookArray.PushBack(book2, allocator);           //添加到数组
    jsonDoc.AddMember("book", bookArray, allocator);
    
    //添加属性
    jsonDoc.AddMember("healthy", true, allocator);  //添加bool类型值
//    jsonDoc.AddMember("sports", NULL, allocator);//添加空值,这里会导致报错
    
    //生成字符串
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    jsonDoc.Accept(writer);
    
    std::string strJson = buffer.GetString();
    log("-----生成的Json:\n%s", strJson.c_str());
    
    //写到文件
    std::string strPath = FileUtils::getInstance()->getWritablePath() + "JsonFile.txt";
    FILE* myFile = fopen(strPath.c_str(), "w");  //windows平台要使用wb
    if (myFile) {
        fputs(buffer.GetString(), myFile);
        fclose(myFile);
    }
    
    /*
     JsonFile.txt
     
     {"name":"qfl","age":20,"letter":["a","b","c"],"location":{"province":"fujian","city":"xiamen","number":16},"book":[{"name":"book1","isbn":"123"},{"name":"book2","isbn":"456"}],"healthy":true}
     */
    log("-----读取Json内容:");
    //从文件中读取（注意和上面分开，不能确定文件是否生成完毕，这里读取可能有问题）
    rapidjson::Document newDoc;
    myFile = fopen(strPath.c_str(), "r");   //windows平台使用rb
    if (myFile) {
        rapidjson::FileStream inputStream(myFile);  //创建一个输入流
        newDoc.ParseStream<0>(inputStream); //将读取的内容转换为dom元素
        fclose(myFile); //关闭文件，很重要
    }
    //判断解析从流中读取的字符串是否有错误
    if (newDoc.HasParseError()) {
        log("Json Parse error:%d", newDoc.GetParseError()); //打印错误编号
    }
    else {
        //获取json串中的数据
        //先判断是否有这个字段，如果使用不存在的key去取值会导致直接崩溃
        if (newDoc.HasMember("name")) {
            log("name:%s", newDoc["name"].GetString()); //必须要获取对应的数据类型，rapidjson不会帮你转换类型
        }
        else {}
        
        if (newDoc.HasMember("age")) {
            log("age:%d", newDoc["age"].GetInt());  //获取正确的类型
        }
        else {}
        
        if (newDoc.HasMember("letter")) {
            rapidjson::Value letter;    //使用一个新的rapidjson::Value来存放array的内容
            letter = newDoc["letter"];
            
            //确保它是一个Array，而且有内容
            if (letter.IsArray() && !letter.Empty()) {
                //遍历Array中的内容
                for (rapidjson::SizeType i = 0; i < letter.Size(); i++) {
                    log("letter:%s", letter[i].GetString());
                }
            }
            else {}
        }
        else {}
        
        if (newDoc.HasMember("location")) {
            rapidjson::Value location;      //使用一个新的rapidjson::Value来存放object
            location = newDoc["location"];
            
            //确保它是一个Object
            if (location.IsObject()) {
                
                if (location.HasMember("province")) {
                    log("location:province:%s", location["province"].GetString());
                }
                else {}
                if (location.HasMember("city")) {
                    log("location:city:%s", location["city"].GetString());
                }
                else {}
                if (location.HasMember("number")) {
                    log("location:number:%d", location["number"].GetInt());
                }
                else {}
            }
            else {}
        }
        else {}
        
        //book是一个包含了2个object的array。按照上面的步骤来取值就行
        if (newDoc.HasMember("book")) {
            rapidjson::Value book;
            book = newDoc["book"];
            
            //先取Array
            if (book.IsArray() && !book.Empty()) {
                
                rapidjson::Value tempBook;
                for (rapidjson::SizeType i = 0; i < book.Size(); i++) {
                    tempBook = book[i]; //Array中每个元素又是一个Object
                    
                    if (tempBook.IsObject()) {
                        
                        if (tempBook.HasMember("name") && tempBook.HasMember("isbn")) {
                            log("book:%d:name:%s, isbn:%s", i, tempBook["name"].GetString(), tempBook["isbn"].GetString());
                        }
                        else {}
                    }
                    else {}
                }
            }
            else {}
        }
        else {}
        
        if (newDoc.HasMember("healthy")) {
            if (newDoc["healthy"].GetBool()) {
                log("healthy:true");
            }
            else {
                log("healthy:false");
            }
        }
        else {}
    }
}

