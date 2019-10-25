#include <iostream> 
#include <string>
#include <limits>
#include "sqlite3.h"


using namespace std;

sqlite3* DB;
int exitMessage;
char* messaggeError;
int res;

void menuLoop();
void yesno();
int callback(void *NotUsed, int argc, char **argv, char **azColName);
int callback2(void *NotUsed, int argc, char **argv, char **azColName);
void createTable();
void printStudents();
void printStudent(string &studentID);
void addStudent();
void deleteStudent();
void editStudent();
void sqlSearch(const string StudentID);
void breakEdit(string StudentID);
void breakAdd(string StudentID);

int main()
{
	int rc;
	rc = sqlite3_open("students.db", &DB);

	if (rc) {
		// Show an error message
		cout << "DB Error: " << sqlite3_errmsg(DB) << endl;
		// Close the connection
		sqlite3_close(DB);
		// Return an error
		return(1);
	}
	createTable();
	menuLoop();
	sqlite3_close(DB);
	return (0);
}

void menuLoop()
{
	while (true)
	{
		std::cout << "Write 1 to list students, 2 to add student, 3 to delete student, 4 to edit student q to exit: ";
		string choice;
		cin >> choice;
		if (choice == "1") printStudents();
		else if (choice == "2")  addStudent();
		else if (choice == "3")  deleteStudent();
		else if (choice == "4")  editStudent();
		else if (choice == "q")  break;

	}
}

int callback(void *NotUsed, int argc, char **argv, char **azColName) {

	// int argc: holds the number of results
	// (array) azColName: holds each column returned
	// (array) argv: holds each value

	for (int i = 0; i < argc; i++) {

		// Show column name, value, and newline
		cout << azColName[i] << ": " << argv[i] << endl;

	}

	// Insert a newline
	cout << endl;

	// Return successful
	return 0;
}

int callback2(void *NotUsed, int argc, char **argv, char **azColName) 
{
	// int argc: holds the number of results
	// (array) azColName: holds each column returned
	// (array) argv: holds each value

	// Return first value
	res = atoi(argv[0]);
	return 0;
}

void yesno() //user confirmation
{
	cout << string(2, '\n');
	string choice;
	cout << "y to delete, n to exit: ";
	cin >> choice;
	cout << string(2, '\n');
	if (choice == "y") return;
	if (choice == "n") menuLoop();
	cout << string(2, '\n');
}

void createTable() 
{
	string sql = "CREATE TABLE Students("
		"StudentID INT PRIMARY KEY     NOT NULL, "
		"LastName           TEXT    NOT NULL, "
		"FirstName          TEXT     NOT NULL, "
		"Birthday           TEXT     NOT NULL, "
		"Address            TEXT);";
	exitMessage = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messaggeError);

	if (exitMessage != SQLITE_OK) { //print error
		string str(messaggeError);
		if (str != "table Students already exists") {
		cout << "***" + str + "***" << endl;
		}
		sqlite3_free(messaggeError);
	}
	else
		cout << "Table created Successfully" << endl;
}

void printStudents() 
{
	cout << string(2, '\n');
	string query = "SELECT * FROM Students;";
	sqlite3_exec(DB, query.c_str(), callback, NULL, NULL);
	cout << string(2, '\n');
}

void printStudent(string &studentID)
{
	cout << string(2, '\n');
	string query = "SELECT * FROM Students WHERE studentID='" + studentID + "';";
	sqlite3_exec(DB, query.c_str(), callback, NULL, NULL);
	cout << string(2, '\n');
}

void addStudent() 
{
	string StudentID,LastName, FirstName, Birthday, Address;
	cout << "Enter studentID:" << endl;
	cin >> StudentID;
	breakAdd(StudentID); //check if StudentID exists
	cout << "Enter student last name:" << endl;
	cin >> LastName;
	cout << "Enter student first name:" << endl;
	cin >> FirstName;
	cout << "Enter student birthday(DD-MM-YYYY):" << endl;
	cin >> Birthday;
	cin.ignore(); //throwing all remaining in cin
	cout << "Enter student adress:" << endl;
	getline(cin, Address);
	
	string sql = "INSERT INTO Students VALUES(" + StudentID + ",'"+ LastName +"', '" + FirstName + "', '" + Birthday + "', '" + Address + "');";
	
	exitMessage = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messaggeError);
	
	if (exitMessage != SQLITE_OK) {
		std::cerr << messaggeError << std::endl;
		sqlite3_free(messaggeError);
	}
	else
		std::cout << "Record created Successfully!" << std::endl;
	
	cout << string(2, '\n');
}

void deleteStudent() {
	string StudentID;
	cout << "Enter studentID to delete: ";
	cin >> StudentID;

	breakEdit(StudentID);
	yesno();

	string sql = "DELETE FROM Students WHERE StudentID = "+ StudentID +";";
	exitMessage = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messaggeError);
	if (exitMessage != SQLITE_OK) {
		std::cerr << "Error DELETE" << std::endl;
		sqlite3_free(messaggeError);
	}
	else
		std::cout << "Record deleted Successfully!" << std::endl;

	cout << string(2, '\n');
}

void sqlEdit(string type, string StudentID)
{
	cout << "Type new "+type+": ";
	string text;
	cin.ignore();
	std::getline(std::cin, text);
	string sql = ("UPDATE Students SET "+type+" = '" + text + "' WHERE StudentID = " + StudentID + " ;");

	exitMessage = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messaggeError);
	if (exitMessage != SQLITE_OK) {
		std::cerr << "Error edit" << std::endl;
		sqlite3_free(messaggeError);
	}
	else
		std::cout << "Record edited Successfully!" << std::endl;
}

void editStudent()
{
	cout << "Enter studentId to edit: ";
	string studentID;
	cin >> studentID;

	breakEdit(studentID);

	printStudent(studentID);
	cout << "1 to edit lastname, 2 to edit firstname, 3 to edit birthday, 4 to edit address, q to quit prompt: ";
	string choice;
	cin >> choice;
	
	if (choice == "1") sqlEdit("LastName", studentID);
	else if (choice == "2")  sqlEdit("FirstName", studentID);
	else if (choice == "3")  sqlEdit("Birthday", studentID);
	else if (choice == "4")  sqlEdit("Address", studentID);
	else if (choice == "q")  return;

	cout << string(2, '\n');
}

void sqlSearch(string StudentID)
{
	string sql = "SELECT EXISTS(SELECT 1 FROM Students WHERE StudentID =" + StudentID + ");";
	sqlite3_exec(DB, sql.c_str(), callback2, NULL, NULL);
}

void breakEdit(string StudentID)
{
	sqlSearch(StudentID);
	if (res == 0)
	{
		cout << string(2, '\n');
		cout << "There is no record with StudentID: " + StudentID + "" << endl;
		cout << string(2, '\n');
		menuLoop();
	}
	
}

void breakAdd(string StudentID)
{
	sqlSearch(StudentID);
	if (res == 1)
	{
		cout << string(2, '\n');
		cout << "There is record with StudentID: " + StudentID + "" << endl;
		cout << string(2, '\n');
		menuLoop();
	}
}