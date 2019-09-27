

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

void printCourse(const Course &course);
void printCourses(vector<Course> courses);
Dept GetDeptFromCollege(const College &college, string deptName);
void search(const College &college);
void printCoursesSatisfacory(const College &college);
vector<Course> FindCourses(const College &college, Course::GradingType grading);
vector<Course> FindCourses(const Dept &dept, Course::GradingType grading);
void printCoursesWithoutStats(vector<Course> courses);
void printCoursesWithDFW(const College &college);
void printCoursesWithB(const College &college);
vector<Course> FindCoursesDFW(const College &college, double threshold);
vector<Course> FindCoursesDFW(const Dept &dept, double threshold);
vector<Course> FindCoursesB(const College &college, double threshold);
vector<Course> FindCoursesB(const Dept &dept, double threshold);

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
void printCollegeSummary(const College &college)
{
    cout << "** College of " << college.Name << "," << college.Semester << " " << college.Year << " **" << endl;
    cout << "# of courses taught: " << college.NumCourses() << endl;
    cout << "# of students taught: " << college.NumStudents() << endl;

    GradeStats gs = GetGradeDistribution(college);
    cout << "grade distribution (A-F):" << gs.PercentA << "% " << gs.PercentB << "% " << gs.PercentC << "% " << gs.PercentD << "% " << gs.PercentF << "%" << endl;

    int dfw;
    int n;
    cout << "DFW rate: " << GetDFWRate(college, dfw, n) << "%" << endl;
}

/**
 * Allows user to enter commands to be processed. Prints the search result of the college or department
 * 
 * @param college Initialized college object
 */

void printSummaryResult(const College &college)
{
    string command;
    cout << "dept name, or all?";
    cin >> command;

    if (command == "all")
    {
        vector<Dept> dt = college.Depts;
        sort(dt.begin(), dt.end(), [](const Dept &d1, const Dept &d2) { return d1.Name < d2.Name; }); // sort dept object using dept names
        for (const Dept &dept : dt)
        {
            cout << dept.Name << ":" << endl;
            cout << " # of courses taught: " << dept.NumCourses() << endl;
            cout << " # of students taught: " << dept.NumStudents() << endl;

            GradeStats gs = GetGradeDistribution(dept);
            cout << " grade distribution (A-F):" << gs.PercentA << " " << gs.PercentB << " " << gs.PercentC << " " << gs.PercentD << " " << gs.PercentF << endl;

            int dfw;
            int n;
            cout << " DFW rate: " << GetDFWRate(dept, dfw, n) << "%" << endl;
        }
    }
    else
    {
        cout << command << ":" << endl;
        Dept dt = GetDeptFromCollege(college, command);
        cout << " # of courses taught: " << dt.NumCourses() << endl;
        cout << " # of students taught: " << dt.NumStudents() << endl;

        GradeStats gs = GetGradeDistribution(dt);
        cout << " grade distribution (A-F):" << gs.PercentA << " " << gs.PercentB << " " << gs.PercentC << " " << gs.PercentD << " " << gs.PercentF << endl;

        int dfw;
        int n;
        cout << " DFW rate: " << GetDFWRate(dt, dfw, n) << "%" << endl;
    }
}

/**
 * Allows user to enter commands to be processed. These commands are then
 * directed to their respective methods to process and print the output.
 * 
 * @param college Initialized college object
 */
void executeUserCommands(const College &college)
{
    while (true)
    {
        string command;
        cout << "Enter a command> ";
        cin >> command;
        if (command == "summary")
        {
            // TODO: create function printDeptSummary
            printSummaryResult(college);
        }
        else if (command == "search")
        {
            search(college);
        }
        else if (command == "satisfactory")
        {
            printCoursesSatisfacory(college);
        }
        else if (command == "dfw")
        {
            printCoursesWithDFW(college);
        }
        else if (command == "letterB")
        {
            printCoursesWithB(college);
        }
        else if (command == "average")
        {
            // TODO: create function printDeptAverage
        }
        else if (command == "#")
        {
            // cout << "Exiting\n";
            return;
        }
        else
            cout << "**unknown command" << endl;
    }
}

/**
 * Executes the search command given by user. Takes command line inputs
 * from user relevant to the search and prints the results
 * 
 * @param college Initialized college object
 */
void search(const College &college)
{
    cout << "dept name, or all? ";

    string dept;
    cin >> dept;

    string instructorPrefix;
    int courseNum;
    cout << "course # or instructor prefix? ";
    cin >> instructorPrefix;

    stringstream ss(instructorPrefix); // create stringstream object
    ss >> courseNum;                   // try to convert input to a course #:

    vector<Course> courses;
    if (ss.fail())
    { // conversion failed, input is not numeric
        if (dept == "all")
        { // instructor from college
            courses = FindCourses(college, instructorPrefix);
        }
        else
        { // instructor from specific department
            Dept department = GetDeptFromCollege(college, dept);
            courses = FindCourses(department, instructorPrefix);
        }
    }
    else
    { // conversion worked, courseNum contains numeric value
        if (dept == "all")
        { // course from college
            courses = FindCourses(college, courseNum);
        }
        else
        { // course from specific department
            Dept department = GetDeptFromCollege(college, dept);
            courses = FindCourses(department, courseNum);
        }
    }

    printCourses(courses);
}

/**
 * Finds and prints the courses with grading satisfactory
 * and prints them in ascending order by course number.
 */
void printCoursesSatisfacory(const College &college)
{
    cout << "dept name, or all? ";

    string dept;
    cin >> dept;

    vector<Course> courses;
    if (dept == "all")
    {
        courses = FindCourses(college, Course::GradingType::Satisfactory);
    }
    else
    {
        Dept department = GetDeptFromCollege(college, dept);
        courses = FindCourses(department, Course::GradingType::Satisfactory);
    }
    printCoursesWithoutStats(courses);
}

/**
 * Prints the course with user input dfw threshold sorted
 * in descending order of dfw rate.
 */
void printCoursesWithDFW(const College &college)
{
    cout << "dept name, or all? ";

    string dept;
    cin >> dept;

    double dfw;
    cout << "dfw threshold? ";
    cin >> dfw;

    vector<Course> courses;
    if (dept == "all")
    {
        courses = FindCoursesDFW(college, dfw);
    }
    else
    {
        Dept department = GetDeptFromCollege(college, dept);
        courses = FindCoursesDFW(department, dfw);
    }

    //requires sorting

    printCourses(courses);
}

/**
 * Prints the course with user input B threshold. Sorted
 * in descending order of B percentage.
 */
void printCoursesWithB(const College &college)
{
    cout << "dept name, or all? ";

    string dept;
    cin >> dept;

    double b;
    cout << "letter B threshold? ";
    cin >> b;

    vector<Course> courses;
    if (dept == "all")
    {
        courses = FindCoursesB(college, b);
    }
    else
    {
        Dept department = GetDeptFromCollege(college, dept);
        courses = FindCoursesB(department, b);
    }

    //requires sorting

    printCourses(courses);
}

/**
 * Prints the details of the course given.
 * 
 * @param course An initialised course object
 */
void printCourse(const Course &course)
{
    cout << course.Dept << " " << course.Number << " (section " << course.Section << "): " << course.Instructor << endl;
    cout << " # students: " << course.getNumStudents() << endl;
    string grading[] = {"letter", "satisfactory", "unknown"};

    cout << " course type: " << grading[course.getGradingType()] << endl;

    GradeStats gs = GetGradeDistribution(course);
    cout << " grade distribution (A-F):" << gs.PercentA << "% " << gs.PercentB << "% " << gs.PercentC << "% " << gs.PercentD << "% " << gs.PercentF << "%" << endl;

    int dfw;
    int n;
    cout << " DFW rate: " << GetDFWRate(course, dfw, n) << "%" << endl;
}

/**
 * Prints the details of the courses given in the vector
 * 
 * @param courses A vector consisting of course objects
 */
void printCourses(vector<Course> courses)
{
    for (const Course &course : courses)
    {
        printCourse(course);
    }
}

/**
 * Prints the details of the course given without any grading stats
 * 
 * @param course An initialised course object
 */
void printCoursesWithoutStats(vector<Course> courses)
{
    for (const Course &course : courses)
    {
        cout << course.Dept << " " << course.Number << " (section " << course.Section << "): " << course.Instructor << endl;
        cout << " # students: " << course.getNumStudents() << endl;
        string grading[] = {"letter", "satisfactory", "unknown"};

        cout << " course type: " << grading[course.getGradingType()] << endl;
    }
}

/**
 * Find Department object from collage 
 * 
 * @param college   An initialized instance of College
 * @param deptName  name of the department you want
 */
Dept GetDeptFromCollege(const College &college, string deptName)
{
    for (const Dept &dept : college.Depts)
    {
        if (dept.Name == deptName) // match:
        {
            return dept;
        }
    }
    return Dept();
}

/**
 * Searches all the courses in the department that matches that has the grading type
 * 
 * @param dept      Reference to the Dept that should be searched.
 * @param grading   Grading type of the course
 * @return          If none are found, then the returned vector is empty.  If
 *                  one or more courses are found, copies of the course objects
 *                  are returned in a vector, with the courses appearing in 
 *                  ascending order by course number.  If two courses have the 
 *                  same course number, they are given in ascending order by 
 *                  section number.  Note that courses are NOT sorted by 
 *                  instructor name.
 */
vector<Course> FindCourses(const Dept &dept, Course::GradingType grading)
{
    vector<Course> courses;

    //
    // looking for 1 or more courses that match the grading:
    //
    for (const Course &course : dept.Courses)
    {
        if (course.getGradingType() == grading) // match:
        {
            courses.push_back(course);
        }
    }

    //
    // sort the vector, first by course number then by section number:
    //
    if (courses.size() > 1) // not required, just a tiny optimization:
    {
        sort(courses.begin(), courses.end(),
             [](const Course &c1, const Course &c2) {
                 return (c1.Section < c2.Section);
             });
    }

    return courses;
}

/**
 * Searches all the courses in the department that matches that has the grading type
 * 
 * @param college   Reference to the College that should be searched.
 * @param grading   Grading type of the course
 * @return          If none are found, then the returned vector is empty.  If
 *                  one or more courses are found, copies of the course objects
 *                  are returned in a vector, with the courses appearing in 
 *                  ascending order by course number.  If two courses have the 
 *                  same course number, they are given in ascending order by 
 *                  section number.  Note that courses are NOT sorted by 
 *                  instructor name.
 */
vector<Course> FindCourses(const College &college, Course::GradingType grading)
{
    vector<Course> courses;
    //
    // For each college, search for courses that match and collect them all
    // in a single vector:
    //
    for (const Dept &dept : college.Depts)
    {
        vector<Course> onedept = FindCourses(dept, grading);

        for (const Course &c : onedept)
        {
            courses.push_back(c);
        }
    }

    //
    // now sort the courses (if any) by dept, course #, and section #:
    //
    if (courses.size() > 1) // not required, just a tiny optimization:
    {
        sort(courses.begin(), courses.end(),
             [](const Course &c1, const Course &c2) {
                 if (c1.Dept < c2.Dept)
                     return true;
                 else if (c1.Dept > c2.Dept)
                     return false;
                 else // same dept, look at course #:
                     if (c1.Number < c2.Number)
                     return true;
                 else if (c1.Number > c2.Number)
                     return false;
                 else // same course #, look at section #:
                     if (c1.Section < c2.Section)
                     return true;
                 else
                     return false;
             });
    }

    return courses;
}

/**
 * Searches all the courses in the department that has a DFW greater than given
 * threshold
 * 
 * @param dept          Reference to the Dept that should be searched.
 * @param threshold     Minimum threshold required
 * @return              If none are found, then the returned vector is empty.  If
 *                      one or more courses are found, copies of the course objects
 *                      are returned in a vector, with the courses appearing in 
 *                      decesending order by dfw rate. If two courses have same rate 
 *                      then they are sorted ascending order of department. If two 
 *                      courses have the same department number. Then sorted by 
 *                      course number. If the have the same course number, they 
 *                      are given in ascending order by section number.  Note 
 *                      that courses are NOT sorted by instructor name.
 */
vector<Course> FindCoursesDFW(const Dept &dept, double threshold)
{
    vector<Course> courses;

    //
    // looking for 1 or more courses that match has the threshold:
    //
    for (const Course &course : dept.Courses)
    {
        int dfw;
        int n;
        double dfwr = GetDFWRate(course, dfw, n);
        if (dfwr >= threshold)
        {
            courses.push_back(course);
        }
    }

    //
    // sort the vector, first by dfw then by course number then by section number:
    //
    if (courses.size() > 1) // not required, just a tiny optimization:
    {
        sort(courses.begin(), courses.end(), [](const Course &c1, const Course &c2) {
            int dfw;
            int n;
            double c1dfw = GetDFWRate(c1, dfw, n);
            double c2dfw = GetDFWRate(c2, dfw, n);
            if (c1dfw > c2dfw) 
                return true;
            else if (c1dfw<c2dfw)
                return false;
            else // same dfw, look at dept
                if (c1.Dept < c2.Dept)
                    return true;
                else if (c1.Dept > c2.Dept)
                    return false;
                else // same dept, look at course #:
                    if (c1.Number < c2.Number)
                        return true;
                    else if (c1.Number > c2.Number)
                        return false;
                    else // same course #, look at section #:
                        if (c1.Section < c2.Section)
                            return true;
                        else
                            return false;
        });
    }

    return courses;
}

/**
 * Searches all the courses in the college that has a DFW greater than given
 * threshold
 * 
 * @param college       Reference to the College that should be searched.
 * @param threshold     Minimum threshold required
 * @return              If none are found, then the returned vector is empty.  If
 *                      one or more courses are found, copies of the course objects
 *                      are returned in a vector, with the courses appearing in 
 *                      decesending order by dfw rate. If two courses have same rate 
 *                      then they are sorted ascending order of department. If two 
 *                      courses have the same department number. Then sorted by 
 *                      course number. If the have the same course number, they 
 *                      are given in ascending order by section number.  Note 
 *                      that courses are NOT sorted by instructor name.
 */
vector<Course> FindCoursesDFW(const College &college, double threshold)
{
    vector<Course> courses;

    //
    // For each college, search for courses that match and collect them all
    // in a single vector:
    //
    for (const Dept &dept : college.Depts)
    {
        vector<Course> onedept = FindCoursesDFW(dept, threshold);

        for (const Course &c : onedept)
        {
            courses.push_back(c);
        }
    }

    //
    // sort the vector, first by dfw then by course number then by section number
    //
    if (courses.size() > 1) // not required, just a tiny optimization:
    {
        sort(courses.begin(), courses.end(), [](const Course &c1, const Course &c2) {
            int dfw;
            int n;
            double c1dfw = GetDFWRate(c1, dfw, n);
            double c2dfw = GetDFWRate(c2, dfw, n);
            if (c1dfw > c2dfw) 
                return true;
            else if (c1dfw<c2dfw)
                return false;
            else // same dfw, look at dept
                if (c1.Dept < c2.Dept)
                    return true;
                else if (c1.Dept > c2.Dept)
                    return false;
                else // same dept, look at course #:
                    if (c1.Number < c2.Number)
                        return true;
                    else if (c1.Number > c2.Number)
                        return false;
                    else // same course #, look at section #:
                        if (c1.Section < c2.Section)
                            return true;
                        else
                            return false;
        });
    }

    return courses;
}

/**
 * Searches all the courses in the department that has a B Percentile than given
 * threshold
 * 
 * @param college       Reference to the College that should be searched.
 * @param threshold     Minimum threshold required
 * @return              If none are found, then the returned vector is empty.  If
 *                      one or more courses are found, copies of the course objects
 *                      are returned in a vector, with the courses appearing in 
 *                      decesending order by B perecentage. If two courses have same 
 *                      percentage then they are sorted ascending order of department.
 *                      If two courses have the same department number. Then sorted 
 *                      by course number. If the have the same course number, they 
 *                      are given in ascending order by section number.Note that 
 *                      courses are NOT sorted by instructor name.
 */
vector<Course> FindCoursesB(const Dept &dept, double threshold)
{
    vector<Course> courses;

    //
    // looking for 1 or more courses that match has the threshold:
    //
    for (const Course &course : dept.Courses)
    {
        GradeStats gs = GetGradeDistribution(course);
        if (gs.PercentB >= threshold)
        {
            courses.push_back(course);
        }
    }

    //
    // sort the vector, first by dfw then by course number then by section number:
    //
    if (courses.size() > 1) // not required, just a tiny optimization:
    {
        sort(courses.begin(), courses.end(), [](const Course &c1, const Course &c2) {
            double c1b = GetGradeDistribution(c1).PercentB;
            double c2b = GetGradeDistribution(c2).PercentB;
            if (c1b > c2b) 
                return true;
            else if (c1b < c2b)
                return false;
            else // same dfw, look at dept
                if (c1.Dept < c2.Dept)
                    return true;
                else if (c1.Dept > c2.Dept)
                    return false;
                else // same dept, look at course #:
                    if (c1.Number < c2.Number)
                        return true;
                    else if (c1.Number > c2.Number)
                        return false;
                    else // same course #, look at section #:
                        if (c1.Section < c2.Section)
                            return true;
                        else
                            return false;
        });
    }

    return courses;
}

/**
 * Searches all the courses in the college that has a B Percentile than given
 * threshold
 * 
 * @param college       Reference to the College that should be searched.
 * @param threshold     Minimum threshold required
 * @return              If none are found, then the returned vector is empty.  If
 *                      one or more courses are found, copies of the course objects
 *                      are returned in a vector, with the courses appearing in 
 *                      decesending order by B perecentage. If two courses have same 
 *                      percentage then they are sorted ascending order of department.
 *                      If two courses have the same department number. Then sorted 
 *                      by course number. If the have the same course number, they 
 *                      are given in ascending order by section number.Note that 
 *                      courses are NOT sorted by instructor name.
 */
vector<Course> FindCoursesB(const College &college, double threshold)
{
    vector<Course> courses;

    //
    // For each college, search for courses that match and collect them all
    // in a single vector:
    //
    for (const Dept &dept : college.Depts)
    {
        vector<Course> onedept = FindCoursesB(dept, threshold);

        for (const Course &c : onedept)
        {
            courses.push_back(c);
        }
    }

    //
    // sort the vector, first by dfw then by course number then by section number
    //
    if (courses.size() > 1) // not required, just a tiny optimization:
    {
        sort(courses.begin(), courses.end(), [](const Course &c1, const Course &c2) {
            double c1b = GetGradeDistribution(c1).PercentB;
            double c2b = GetGradeDistribution(c2).PercentB;
            if (c1b > c2b) 
                return true;
            else if (c1b < c2b)
                return false;
            else // same dfw, look at dept
                if (c1.Dept < c2.Dept)
                    return true;
                else if (c1.Dept > c2.Dept)
                    return false;
                else // same dept, look at course #:
                    if (c1.Number < c2.Number)
                        return true;
                    else if (c1.Number > c2.Number)
                        return false;
                    else // same course #, look at section #:
                        if (c1.Section < c2.Section)
                            return true;
                        else
                            return false;
        });
    }

    return courses;
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
