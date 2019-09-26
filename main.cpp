

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
using namespace std;

// includes for gradeutil
#include "gradeutil.h"

College InputGradeData(string filename)
{
    College college;
    ifstream file(filename);
    string line, value;

    if (!file.good())
    {
        cout << "**Error: unable to open input file '" << filename << "'." << endl;
        return college;
    }

    // first line contains semester,year
    getline(file, line);
    stringstream ss(line);

    getline(ss, college.Name, ',');
    getline(ss, college.Semester, ',');
    getline(ss, value);
    college.Year = stoi(value);

    // second line contains column headers --- skip
    getline(file, line);

    //
    // now start inputting and parse course data:
    //

    while (getline(file, line))
    {
        Course c = ParseCourse(line);

        //
        // search for correct dept to ask course to, otherwise create a new dept:
        //
        auto dept_iter = std::find_if(college.Depts.begin(),
                                      college.Depts.end(),
                                      [&](const Dept &d) {
                                          return (d.Name == c.Dept);
                                      });

        if (dept_iter == college.Depts.end())
        {
            //
            // doesn't exist, so we have to create a new dept
            // and insert course:
            //
            Dept d(c.Dept);

            d.Courses.push_back(c);

            college.Depts.push_back(d);
        }
        else
        {
            // dept exists, so insert course into existing dept:
            dept_iter->Courses.push_back(c);
        }

    } //while

    //
    // done:
    //
    return college;
}

// TODO: define your own functions

/**
 * Prints the summary of the college
 * 
 * @param college Initialized college object
 */
void printCollegeSummary(const College& college){
    cout << "** College of " << college.Name << "," << college.Semester << " " << college.Year << " **\n";
    cout << "# of courses taught: " << college.NumCourses() << "\n";
    cout << "# of students taught: " << college.NumStudents() << "\n";

    GradeStats gs=GetGradeDistribution(college);
    cout << "grade distribution (A-F):" << gs.PercentA << " " << gs.PercentB << " " << gs.PercentC << " " << gs.PercentD << " " << gs.PercentF << "\n";

    int dfw;
    int n;
    cout << "DFW rate: " << GetDFWRate(college,dfw,n) << "%\n";
}


/**
 * Allows user to enter commands to be processed. Prints the search result of the college or department
 * 
 * @param college Initialized college object
 */

void printSummaryResult(const College& college)
{
    string command;
    cout<< "dept name, or all?";
    cin >> command;

    if (command == "all")
    {
        
    }else
    {
        cout<< command << ":" <<endl;
        
    }
    
     
    
}



/**
 * Allows user to enter commands to be processed. These commands are then
 * directed to their respective methods to process and print the output.
 * 
 * @param college Initialized college object
 */
void executeUserCommands(const College& college){
    while(true){
        string command;
        cout << "Enter a command> ";
        cin >> command;
        if (command == "summary"){
            // TODO: create function printDeptSummary
            printSummaryResult(college);

        }else if(command == "search"){
            search(college);
        
        }else if(command == "satisfactory"){
            // TODO: create function printCoursesSatisfacory
        
        }else if(command == "dfw"){
            // TODO: create function printCoursesWithDFW
        
        }else if(command == "letterB"){
            // TODO: create function printCoursesWithB
        
        }else if(command == "average"){
            // TODO: create function printDeptAverage
        
        }else if(command == "#"){
            cout << "Exiting\n";
            return;
        }else 
            cout << "**unknown command\n";

    }
}

void search(const College& college){
    cout << "dept name, or all? ";

    string dept;
    cin >> dept;

    string instructorPrefix;
    int courseNum;
    cout << "course # or instructor prefix? ";
    cin >> instructorPrefix;
    
    stringstream ss(instructorPrefix); // create stringstream object
    ss >> courseNum; // try to convert input to a course #:
    
    vector<Course> courses;
    if ( ss.fail() ){ // conversion failed, input is not numeric
        if (dept == "all"){  // instructor from college
            courses = FindCourses(college,instructorPrefix);
        }else{ // instructor from specific department
            courses = FindCourses(dept,instructorPrefix);
        }
    }else{ // conversion worked, courseNum contains numeric value
        if (dept == "all"){ // course from college
            courses = FindCourses(college,courseNum);
        }else{ // course from specific department
            courses = FindCourses(dept,courseNum);
        }
    }
    
    printCourses(courses);
}

/**
 * Prints the details of the course given.
 * 
 * @param course An initialised course object
 */
void printCourse(const Course& course){
    cout << course.Title << endl;
    cout << " # students: " << course.getNumStudents() << endl;
    cout << " course type: " << course.getGradingType() << endl;
    cout << " grade distribution (A-F): " << course.NumA << " " << course.NumB << " " << course.NumC << " " << course.NumD << " " << course.NumF << "\n";

    int dfw;
    int n;
    cout << "DFW rate: " << GetDFWRate(course,dfw,n) << "%" << endl;
}

/**
 * Prints the details of the courses given in the vector
 * 
 * @param courses A vector consisting of course objects
 */
void printCourses(vector<Course> courses){
    for(const Course& course : courses){
        printCourse(course);
    }
}

/**
 * Find Department object from collage 
 * 
 * @param college An initialized instance of College
 * @param deptName name of the department you want
 */
Dept GetDeptFromCollege(const College& college,string deptName){
    for(const Dept& dept : college.Depts)
    {
        if (dept.Name == deptName)  // match:
        {
            return dept;
        }
    }
}

int main()
{
    string filename;

    cout << std::fixed;
    cout << std::setprecision(2);

    //
    // 1. Input the filename and then the grade data:
    //
    cin >> filename;
    // filename = "fall-2018.csv";

    College college = InputGradeData(filename);

    // 2. print out summary of the college
    printCollegeSummary(college);

    //
    // 3. Start executing commands from the user:
    executeUserCommands(college);
    

    //
    // done:
    //
    return 0;
}
