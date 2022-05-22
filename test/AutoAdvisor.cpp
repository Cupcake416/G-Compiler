const int MAX_STRING_LENGTH = 250;
const int MAX_COURSE_NUM = 110;
int courseCount;
char courseName[27500];
int credit[110];
char depandence[27500];
int grade[110];
char recommendation[27500];
int recommendationCount;
char name[110];

// return true if there exists a course named 'name'
bool find() {
    int i, j, k;
    i = 0;
    bool b;
    while(i < courseCount) {
        if(grade[i] > 0) {
            j = i * MAX_STRING_LENGTH;
            k = 0;
            b = true;
            while(courseName[j] != 0 || name[k] != 0) {
                if(courseName[j] != name[k]) {
                    b = false;
                    break;
                }
                j = j + 1;
                k = k + 1;
            }
            if(b) {
                return true;
            }
        }
        i = i + 1;
    }
    return false;
}

// judge if the course satisfy depandence
bool judge(int index) {
    bool res, partialRes;
    res = false; partialRes = true;
    int i, si;
    i = index * MAX_STRING_LENGTH;
    si = 0;
    if(depandence[i] == 0) {
        return true;
    }
    while(1) {
        if(depandence[i] == 0 || depandence[i] == ';') {
            name[si] = 0;
            partialRes = partialRes && find();
            si = 0;
            res = res || partialRes;
            partialRes = true;
            if(res) {
                return true;
            }
            if(depandence[i] == 0) {
                break;
            }
        } else if(depandence[i] == ',') {
            name[si] = 0;
            partialRes = partialRes && find();
            si = 0;
        } else {
            name[si] = depandence[i];
            si = si + 1;
        }

        i = i + 1;
    }
    return false;
}

int main() {
    char ch;
    int i;

    // handle input
    while(1) {
        // input course name
        i = MAX_STRING_LENGTH * courseCount;
        while(1) {
            scan(ch);
            if(ch == '|' || ch == '\n') {
                break;
            }
            courseName[i] = ch;
            i = i + 1;
        } 
        courseName[i] = 0;

        // end input
        if(ch == '\n') {
            break;
        }

        // input credit
        scan(ch);
        credit[courseCount] = ch - '0';
        scan(ch);

        // input depandence
        i = MAX_STRING_LENGTH * courseCount;
        while(1) {
            scan(ch);
            if(ch == '|' || ch == '\n') {
                break;
            }
            depandence[i] = ch;
            i = i + 1;
        } 
        depandence[i] = 0;

        // input grade
        // A = 4, B = 3, C = 2, D = 1, F = 0, none = -1
        scan(ch);
        if(ch >= 'A' && ch <= 'F') {
            grade[courseCount] = 4 - (ch - 'A');
            if(ch == 'F') {
                grade[courseCount] = 0;
            }
            scan(ch);
        } else {
            grade[courseCount] = -1;
        }
        courseCount = courseCount + 1;
    }

    int hoursAttempted, hoursCompleted, creditsRemaining;
    hoursAttempted = 0;
    hoursCompleted = 0;
    creditsRemaining = 0;
    double gpa; gpa = 0;

    i = 0;
    while(i < courseCount) {
        // printf("%d %d\n", credit[i], grade[i]);
        if(grade[i] >= 0) {
            hoursAttempted = hoursAttempted + credit[i];
            gpa = gpa + credit[i] * grade[i];
            if(grade[i] > 0) {
                hoursCompleted = hoursCompleted + credit[i];
            }
        }
        if(grade[i] <= 0) {
            creditsRemaining = creditsRemaining + credit[i];
        }
        if(grade[i] <= 0 && judge(i)) {
            recommendation[recommendationCount] = i;
            recommendationCount = recommendationCount + 1;
        }
        i = i + 1;
    }
    if(hoursAttempted > 0) {
        gpa = gpa / hoursAttempted;
    }
    
    int int_gpa;
    int_gpa = (gpa + 0.05) * 10;
    print("GPA: ", int_gpa / 10, '.', int_gpa % 10, "\n");
    print("Hours Attempted: ", hoursAttempted, "\n");
    print("Hours Completed: ", hoursCompleted, '\n');
    print("Credits Remaining: ", creditsRemaining, '\n');
    print("\nPossible Courses to Take Next\n");
    if(recommendationCount > 0) {
        i = 0;
        while(i < recommendationCount) {
            int j;
            print("  ");
            j = recommendation[i] * MAX_STRING_LENGTH;
            while(courseName[j] > 0)
            {
                print(courseName[j]);
                j = j + 1;
            }
            print("\n");
            i = i + 1;
        }
    } else {
        if(creditsRemaining == 0) {
            print("  None - Congratulations!\n");
        }
    }
    return 0;
}