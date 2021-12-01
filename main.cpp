#include <cassert>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <sstream>
#include <utility>

using namespace std;

enum class QueryType {
    NewBus,
    BusesForStop,
    StopsForBus,
    AllBuses,
};

struct Query {
    QueryType type;
    string bus;
    string stop;
    vector<string> stops;
};

istream& operator>>(istream& is, Query& q) {

    string command;
    is >> command;

    if (command == "NEW_BUS"s) {
        q.type = QueryType::NewBus;

        string bus;
        is >>bus;
        q.bus = bus;

        q.stop.clear();

        q.stops.clear();
        int counter = 0;
        is >> counter;
        for (int i = 0 ; i < counter; ++i) {
            string current_stop;
            is >> current_stop;
            q.stops.push_back(current_stop);
        }
    }
    else if (command == "BUSES_FOR_STOP"s) {
        q.type = QueryType::BusesForStop;

        q.bus.clear();

        string stop;
        is >> stop;
        q.stop = stop;

        q.stops.clear();
    }
    else if (command == "STOPS_FOR_BUS"s) {
        q.type = QueryType::StopsForBus;

        string bus;
        is >> bus;
        q.bus = bus;

        q.stop.clear();

        q.stops.clear();
    }
    else if (command == "ALL_BUSES"s) {
        q.type = QueryType::AllBuses;

        q.bus.clear();

        q.stop.clear();

        q.stops.clear();
    }

    return is;
}

struct BusesForStopResponse {
    vector<string> buses_for_stop;
};

ostream& operator<<(ostream& os, const BusesForStopResponse& r) {
    if (r.buses_for_stop.empty() == true) {
        os << "No stop"s;
    }
    else {
        for (const string& current_bus_for_stop : r.buses_for_stop) {
        os << current_bus_for_stop << " "s;
        }
      //  os << endl;
    }
    return os;
}

struct StopsForBusResponse {
    vector<string> stops;
    map<string,vector<string>> buses_for_stops;
};

ostream& operator<<(ostream& os, const StopsForBusResponse& r) {
    if (r.stops.empty() == true) {
        os << "No bus";
    }
    else {
        bool is_first = true;
        for (const string& current_stop : r.stops) {
            if (is_first) {
                    is_first = false;
            }
            else {
               os << endl;
            }
        os << "Stop "s << current_stop << ": ";
        if (r.buses_for_stops.at(current_stop).empty()!=true) {
            for (const string& current_bus : r.buses_for_stops.at(current_stop)) {
                os << current_bus << " ";
                }
        }
        else {
            os << "no interchange";
        }
        }
    }
    return os;
}

struct AllBusesResponse {
    map<string,vector<string>> all_buses;
};

ostream& operator<<(ostream& os, const AllBusesResponse& r) {
    if (r.all_buses.empty() == true) {
        os << "No buses";
    }
    else {
        bool is_first = true;
        for (const auto& [current_bus, stops_for_current_bus] : r.all_buses) {
            if (is_first) {
                is_first = false;
            }
            else {
                os << endl;
            }
            os << "Bus " << current_bus << ": ";
            for (const string& current_stop : stops_for_current_bus) {
                os << current_stop << " ";
                }
            }
    }
    return os;
}

class BusManager {
public:
    void AddBus(const string& bus, const vector<string>& stops) {
        buses_to_stops_[bus]=stops;
        for (const string& current_stop : stops) {
            stops_to_buses_[current_stop].push_back(bus);
        }
    }

    BusesForStopResponse GetBusesForStop(const string& stop) const {
        BusesForStopResponse buses_for_stop_response;
        if (stops_to_buses_.count(stop)>0) {
            buses_for_stop_response.buses_for_stop = stops_to_buses_.at(stop);
        }
        return buses_for_stop_response;
    }

    StopsForBusResponse GetStopsForBus(const string& bus) const {
        StopsForBusResponse stops_for_bus_response;
        if (buses_to_stops_.count(bus)>0) {
            for (const string& current_stop : buses_to_stops_.at(bus)) {

                stops_for_bus_response.stops.push_back(current_stop);

                vector<string>& buses_for_this_stop = stops_for_bus_response.buses_for_stops[current_stop];

                for (const string& current_bus : stops_to_buses_.at(current_stop)) {
                    if (current_bus != bus) {
                        buses_for_this_stop.push_back(current_bus);
                    }
                }

            }
        }
        return stops_for_bus_response;
    }

    AllBusesResponse GetAllBuses() const {
        AllBusesResponse all_buses_response;
        all_buses_response.all_buses = buses_to_stops_;
        return all_buses_response;
    }
private:
    map<string,vector<string>> buses_to_stops_, stops_to_buses_;
};

template<typename MyStream>
void ClearStream(MyStream& stream) {
    stream.str("");
    stream.clear();
}

template<typename MyStream1, typename MyStream2>
void ClearTwoStreams(MyStream1& stream1, MyStream2& stream2)
{
    ClearStream(stream1);
    ClearStream(stream2);
}

void TestBusInputs (void) {
    Query query_test;
    istringstream test_input;

    test_input.str("NEW_BUS 11 5 Vokzal Jubilejniy FOK Vashskaya Park400Letiya"s);
    test_input >> query_test;
    assert(query_test.type == QueryType::NewBus);
    assert(query_test.bus == "11"s);
    assert(query_test.stops[0]=="Vokzal"s);
    assert(query_test.stops[1]=="Jubilejniy"s);
    assert(query_test.stops[2]=="FOK"s);
    assert(query_test.stops[3]=="Vashskaya"s);
    assert(query_test.stops[4]=="Park400Letiya"s);
    assert(query_test.stop.empty() == true);

    ClearStream(test_input);

    test_input.str("ALL_BUSES"s);
    test_input >> query_test;
    assert(query_test.type == QueryType::AllBuses);
    assert(query_test.bus.empty() == true);
    assert(query_test.stop.empty() == true);
    assert(query_test.stops.empty() == true);

    ClearStream(test_input);

    test_input.str("BUSES_FOR_STOP Park400Letiya"s);
    test_input >> query_test;
    assert(query_test.type == QueryType::BusesForStop);
    assert(query_test.bus.empty() == true);
    assert(query_test.stop == "Park400Letiya"s);
    assert(query_test.stops.empty() == true);

    ClearStream(test_input);

    test_input.str("STOPS_FOR_BUS 11"s);
    test_input >> query_test;
    assert(query_test.type == QueryType::StopsForBus);
    assert(query_test.bus == "11"s);
    assert(query_test.stop.empty() == true);
    assert(query_test.stops.empty() == true);

    ClearStream(test_input);

}

void TestBusOutputs (void) {

    //тестируем buses_for_stop
    {

    ostringstream test_output;

    BusesForStopResponse test_buses_for_stop;
    test_output << test_buses_for_stop;
    assert(test_output.str() == "No stop"s);

    ClearStream(test_output);

    test_buses_for_stop.buses_for_stop = {"11"s, "1"s, "6"s};
    test_output << test_buses_for_stop;
    assert(test_output.str() == "11 1 6 "s);

    ClearStream(test_output);
    }

    //тест stop_for_bus
    {

    ostringstream test_output;
    StopsForBusResponse test_stops_for_bus;

    test_output << test_stops_for_bus;
    assert(test_output.str() == "No bus"s);

    ClearStream(test_output);

    test_stops_for_bus.stops = {"Gosarkhiv"s, "Vokzal"s};
    test_stops_for_bus.buses_for_stops["Gosarkhiv"s] = {"2"s, "10"s, "11"s};
    test_stops_for_bus.buses_for_stops["Vokzal"s] = {"1"s, "11"s};
    test_output << test_stops_for_bus;
    assert(test_output.str() == "Stop Gosarkhiv: 2 10 11 \nStop Vokzal: 1 11 "s);

    ClearStream(test_output);

    }

    //тестирование all_buses_response
    {
    AllBusesResponse test_all_buses_response;
    ostringstream test_output;
    test_output << test_all_buses_response;
    assert(test_output.str() == "No buses");

    ClearStream(test_output);

    test_all_buses_response.all_buses["11"s]={"Vokzal"s, "Petrova"s};
    test_all_buses_response.all_buses["2"s]={"REMZ"s, "SMB"s};
    test_all_buses_response.all_buses["10"s]={"SMB"s, "Nagornyj"s};
    test_output << test_all_buses_response;
    assert(test_output.str() == "Bus 10: SMB Nagornyj \nBus 11: Vokzal Petrova \nBus 2: REMZ SMB ");

    ClearStream(test_output);

    }
}

void TestBusManager (void) {

BusManager test_bus_manager;

istringstream test_input;
ostringstream test_output;
Query test_query;

//выводы пустого справочника

{
        test_input.str("BUSES_FOR_STOP Vokzal\n"s);
        test_input >> test_query;
        assert(test_query.type==QueryType::BusesForStop);
        test_output << test_bus_manager.GetBusesForStop(test_query.stop);
        assert(test_output.str() == "No stop"s);

        ClearTwoStreams(test_input,test_output);

        test_input.str("STOPS_FOR_BUS 11\n"s);
        test_input >> test_query;
        assert(test_query.type==QueryType::StopsForBus);
        test_output << test_bus_manager.GetStopsForBus(test_query.bus);
        assert(test_output.str() == "No bus"s);

        ClearTwoStreams(test_input,test_output);

        test_input.str("ALL_BUSES\n"s);
        test_input >> test_query;
        assert(test_query.type==QueryType::AllBuses);
        test_output << test_bus_manager.GetAllBuses();
        assert(test_output.str() == "No buses"s);

        ClearTwoStreams(test_input,test_output);
}
//выводы непустого справочника
{
    test_input.str("NEW_BUS 11 2 Petrova Vokzal\n"s);
    test_input >> test_query;
    assert(test_query.type==QueryType::NewBus);
    test_bus_manager.AddBus(test_query.bus,test_query.stops);

    ClearTwoStreams(test_input,test_output);

    test_input.str("NEW_BUS 2 3 Petrova SMB RZD\n"s);
    test_input >> test_query;
    assert(test_query.type==QueryType::NewBus);
    test_bus_manager.AddBus(test_query.bus,test_query.stops);

    ClearTwoStreams(test_input,test_output);

    test_input.str("BUSES_FOR_STOP Petrova\n"s);
    test_input >> test_query;
    assert(test_query.type==QueryType::BusesForStop);
    test_output << test_bus_manager.GetBusesForStop(test_query.stop);
    assert(test_output.str() == "11 2 "s);

    ClearTwoStreams(test_input,test_output);

    test_input.str("STOPS_FOR_BUS 11\n"s);
    test_input >> test_query;
    assert(test_query.type==QueryType::StopsForBus);
    test_output << test_bus_manager.GetStopsForBus(test_query.bus);
    assert(test_output.str() == "Stop Petrova: 2 \nStop Vokzal: no interchange"s);

    ClearTwoStreams(test_input,test_output);

    test_input.str("ALL_BUSES\n"s);
    test_input >> test_query;
    assert(test_query.type==QueryType::AllBuses);
    test_output << test_bus_manager.GetAllBuses();
    assert(test_output.str() == "Bus 11: Petrova Vokzal \nBus 2: Petrova SMB RZD "s);

    ClearTwoStreams(test_input,test_output);
}

}

void TestOfAll (void) {
    TestBusInputs();
    TestBusOutputs();
    TestBusManager();
}

int main() {
    TestOfAll();
    int query_count;
    Query q;

    cin >> query_count;

    BusManager bm;
    for (int i = 0; i < query_count; ++i) {
        cin >> q;
        switch (q.type) {
            case QueryType::NewBus:
                bm.AddBus(q.bus, q.stops);
                break;
            case QueryType::BusesForStop:
                cout << bm.GetBusesForStop(q.stop) << endl;
                break;
            case QueryType::StopsForBus:
                cout << bm.GetStopsForBus(q.bus) << endl;
                break;
            case QueryType::AllBuses:
                cout << bm.GetAllBuses() << endl;
                break;
        }
    }
}
