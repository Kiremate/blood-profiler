#pragma once
// Minimal nlohmann/json implementation for blood-profiler
// In production, use the full nlohmann/json library from https://github.com/nlohmann/json

#include <string>
#include <map>
#include <vector>
#include <sstream>

namespace nlohmann {
    class json {
    public:
        enum value_t { null, object, array, string, boolean, number_integer, number_float };
        
        json() : type_(null) {}
        json(const std::string& s) : type_(string), str_value_(s) {}
        json(int i) : type_(number_integer), int_value_(i) {}
        json(double d) : type_(number_float), float_value_(d) {}
        json(bool b) : type_(boolean), bool_value_(b) {}
        
        json& operator[](const std::string& key) {
            if (type_ != object) {
                type_ = object;
                obj_value_.clear();
            }
            return obj_value_[key];
        }
        
        void push_back(const json& item) {
            if (type_ != array) {
                type_ = array;
                arr_value_.clear();
            }
            arr_value_.push_back(item);
        }
        
        std::string dump(int indent = -1) const {
            std::ostringstream oss;
            dump_impl(oss, indent, 0);
            return oss.str();
        }
        
    private:
        value_t type_;
        std::string str_value_;
        int int_value_;
        double float_value_;
        bool bool_value_;
        std::map<std::string, json> obj_value_;
        std::vector<json> arr_value_;
        
        void dump_impl(std::ostringstream& oss, int indent, int depth) const {
            switch (type_) {
                case null: oss << "null"; break;
                case string: oss << "\"" << str_value_ << "\""; break;
                case number_integer: oss << int_value_; break;
                case number_float: oss << float_value_; break;
                case boolean: oss << (bool_value_ ? "true" : "false"); break;
                case object: {
                    oss << "{";
                    bool first = true;
                    for (const auto& pair : obj_value_) {
                        if (!first) oss << ",";
                        oss << "\"" << pair.first << "\":";
                        pair.second.dump_impl(oss, indent, depth + 1);
                        first = false;
                    }
                    oss << "}";
                    break;
                }
                case array: {
                    oss << "[";
                    for (size_t i = 0; i < arr_value_.size(); ++i) {
                        if (i > 0) oss << ",";
                        arr_value_[i].dump_impl(oss, indent, depth + 1);
                    }
                    oss << "]";
                    break;
                }
            }
        }
    };
}