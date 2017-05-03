//
// Created by Olia on 04.04.2017.
//

#include <iostream>
#include <fstream>
#include <math.h>
#include <assert.h>
#include <mutex>
#include <thread>
#include <map>
#include <string>
#include <sstream>

using namespace std;

mutex mx;

double func_calculation(int m, double x1, double x2) {
    double sum1 = 0;
    double sum2 = 0;
    double g;
    for (int i = 1; i <= m; ++i)
    {
        sum1 += i * cos((i + 1) * x1 + 1);
        sum2 += i * cos((i + 1) * x2 + 1);
    }

    g = - sum1 * sum2;

    return g;
}

double integration(double x0, double x, double y0, double y, int m, double pr) {
    assert (m >= 5);
    double sum = 0;
    for (double i = x0; i <= x; i += pr) {
        for (double j = y0; j <= y; j += pr) {
            sum += func_calculation(m, i + pr / 2.0, j + pr / 2.0) * pr * pr;
        }
    }
    return sum;
}

void thread_integration(double x0, double x, double y0, double y, int m, double pr, double* r) {

    auto result = integration(x0, x, y0, y, m, pr);
    lock_guard<mutex> lg(mx);
    *r += result;
}


int main()
{
    string filename;
    cout << "Please enter name of configuration file with extension '.txt':>";
    cin >> filename;
//    filename = "config.txt";
    string line;
    ifstream myfile;
    map<string, string> mp;
    myfile.open(filename);

    if (myfile.is_open())
    {
        while (getline(myfile,line))
        {
            int pos = line.find("=");
            string key = line.substr(0, pos);
            string value = line.substr(pos + 1);
            mp.insert(make_pair(key, value));
        }

        myfile.close();
    }
    else {
        cout << "Error with opening the file!" << endl;
        return 0;
    }

    double abs_pr, rel_pr, x0, x1, y0, y1;
    int m, num_of_threads;
    istringstream s1(mp["absol_pr"]);
        s1 >> abs_pr;
    istringstream s2(mp["rel_pr"]);
        s2 >> rel_pr;
    istringstream s3(mp["x0"]);
        s3 >> x0;
    istringstream s4(mp["x1"]);
        s4 >> x1;
    istringstream s5(mp["y0"]);
        s5 >> y0;
    istringstream s6(mp["y1"]);
        s6 >> y1;
    istringstream s7(mp["m"]);
        s7 >> m;
    istringstream s8(mp["threads"]);
        s8 >> num_of_threads;

    clock_t t1, t2;
    t1 = clock();
    thread threads[num_of_threads];
    double pr = 1E-3;

    double integral = 0;
    double interval_x = (x1 - x0) / num_of_threads;
    double x = x0;
    cout << "  Calculating..." << endl;

    for (int i = 0; i < num_of_threads; ++i)
    {
        threads[i] = thread(thread_integration, x, x + interval_x, y0, y1, m, pr, &integral);
        x += interval_x;
    }

    for (int j = 0; j < num_of_threads; ++j)
    {
        threads[j].join();
    }

    t2 = clock();
    float the_time = float((t2 - t1))/(CLOCKS_PER_SEC / 1000);
    cout << " -------------------------------------\n| Time: " <<  the_time << " ms\n -------------------------------------" << endl;

    double integ = integration(x0, x1, y0, y1, m, pr);
    double absolute = abs(integral - integ);
    double relative = abs(absolute / integ);

    ofstream result;
    result.open("result.txt");
        result << "| Threads result: " << integral << "\n|-----------------------------" << endl;
        result << "| Function result: " << integ << "\n|-----------------------------" << endl;
        result << "| Absolute: " << absolute << endl;
        result << "| Relative: " << relative << "\n|-----------------------------" << endl;
        result << "| Time: " << the_time << " ms\n -----------------------------" << endl;

    cout << "\t|  THREADS result: " << integral << endl;
    cout << "\t|-----------------------------\n";
    cout << "\t| FUNCTION result: " << integ << "\n\t -----------------------------" << endl;

    return 0;
}
