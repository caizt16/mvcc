#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <mutex>

class DataValue {
    public:
        DataValue(int _val, int _time = 0);
        friend std::ostream& operator<<(std::ostream& os, const DataValue& dv);
        int val;
        int time;
    private:
        DataValue();
};

class MVCC {
    public:
        MVCC(int _threads);
        void run();
        void print();
    private:
        MVCC();
        void prepare_data(std::string const& input_file);
        void run_thread(int trans_id);

        int threads;
        std::mutex m;
        std::map<std::string, std::vector<DataValue> > data_map;
        //std::map<std::string, std::vector<std::pair<int, int> > > data_map;
};