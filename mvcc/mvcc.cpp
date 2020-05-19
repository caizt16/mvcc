#include "mvcc.h"

#include "parser.h"

#include <ctime>
#include <string>
#include <fstream>
#include <vector>
#include <thread>
#include <iostream>

std::ostream &operator<<(std::ostream &os, const std::vector<int> &v) {
    os << "[ ";
    for (auto val: v) {
        os << val << " ";
    }
    os << "]";
    return os;
}

std::ostream &operator<<(std::ostream &os, const DataValue &dv) {
    os << dv.val << " " << dv.time;
    return os;
}

std::ostream &operator<<(std::ostream &os, 
const std::map<std::string, std::vector<DataValue> > &data_map) {
    os << "print data map" << std::endl;
    for (auto iter: data_map) {
        os << iter.first << " ";
        for (auto dv: iter.second)
            os << dv << " ";
        os << std::endl;
    }
    return os;
}

DataValue::DataValue(int _val, int _time): val(_val), time(_time)
{
}

DataValue::DataValue() {}

MVCC::MVCC(int _threads): threads(_threads){
}

MVCC::MVCC() {}

void MVCC::prepare_data(std::string const& input_file) {
    std::ifstream fin(input_file);
    std::string data_line;
    std::vector<std::string> data_vec;
    while(std::getline(fin, data_line)) {
        data_vec = split(data_line);
        if (data_vec[0] != "INSERT") {
            std::cout << "invalid data preparation instruction: " << data_line << std::endl;
            return;
        }
        data_map.insert(std::make_pair(data_vec[1], std::vector<DataValue>()));
        data_map[data_vec[1]].emplace_back(std::stoi(data_vec[2]), 0);
    }
    //std::cout << data_map << std::endl;
}

void MVCC::run_thread(int thread_id) {
    std::string input_file = "../inputs/thread_" + std::to_string(thread_id) + ".txt";
    std::ifstream fin(input_file);

    int trans_id;
    std::string trans_line;
    std::vector<std::string> trans_vec;
    clock_t trans_time, op_time;
    std::map<std::string, int> temp_map;
    std::map<std::string, bool> mod_map;

    std::string output_string = "transaction_id,type,time,value\n";
    while(std::getline(fin, trans_line)) {
        trans_vec = split(trans_line);
        if (trans_vec[0] == "BEGIN") {
            trans_id = std::stoi(trans_vec[1]);
            m.lock();
            op_time = clock();
            for (auto it = data_map.begin(); it != data_map.end(); ++it)
            {
                auto temp_data = it->second;
                for (int i = temp_data.size() - 1; i >= 0; --i)
                    if (temp_data[i].time < op_time)
                    {
                        temp_map[it->first] = temp_data[i].val;
                        break;
                    }
            }
            m.unlock();
            trans_time = op_time;
            output_string += std::to_string(trans_id) + ",BEGIN," + std::to_string(op_time) + ",\n";
        } else if (trans_vec[0] == "COMMIT") {
            m.lock();
            for (auto data : mod_map)
            {
                data_map[data.first].emplace_back(temp_map[data.first], op_time);
            }
            op_time = clock();
            m.unlock();
            temp_map.clear();
            mod_map.clear();
            output_string += std::to_string(trans_id) + ",END," + std::to_string(op_time) + ",\n";
        } else if (trans_vec[0] == "READ") {
            op_time = clock();
            output_string += std::to_string(trans_id) + "," + trans_vec[1] + "," + std::to_string(op_time) + "," + std::to_string(temp_map[trans_vec[1]]) + "\n";
        } else if (trans_vec[0] == "SET") {
            trans_vec[1] = trans_vec[1].substr(0, trans_vec[1].size()-1);
            mod_map[trans_vec[1]] = true;
            if (trans_vec[3] == "+") {
                temp_map[trans_vec[1]] = temp_map[trans_vec[2]] + std::stoi(trans_vec[4]);
            } else if (trans_vec[3] == "-") {
                temp_map[trans_vec[1]] = temp_map[trans_vec[2]] - std::stoi(trans_vec[4]);
            }
        } else {
            std::cout << "invalid transaction instruction: " << trans_line << std:: endl;
            return;
        }
    }

    std::string output_file = "../inputs/output_thread_" + std::to_string(thread_id) + ".csv";
    std::ofstream fout(output_file);
    fout << output_string;
}

void MVCC::run() {
    prepare_data("../inputs/data_prepare.txt");
    std::vector<std::thread> thread_vec;
    for (int i = 0; i < threads; ++i) {
        thread_vec.emplace_back(&MVCC::run_thread, this, i+1);
    }
    for (int i = 0; i < threads; ++i) {
        thread_vec[i].join();
    }
}

void MVCC::print() {

}
