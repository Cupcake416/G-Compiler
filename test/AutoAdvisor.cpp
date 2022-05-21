#include <cstdio>
#define MAX_STRING_LENGTH 110
#define MAX_COURSE_NUM 110
int courseCount;
char courseName[MAX_STRING_LENGTH * MAX_COURSE_NUM];
int credit[MAX_COURSE_NUM];
char depandence[MAX_STRING_LENGTH * MAX_COURSE_NUM];
int grade[MAX_COURSE_NUM];
char recommendation[MAX_STRING_LENGTH * MAX_COURSE_NUM];
int recommendationCount;

// return true if there exists a course named 'name'
bool find(char name[MAX_COURSE_NUM]) {
    int i, j, k;
    i = 0;
    bool b;
    while(i < courseCount) {
        if(grade[i] > 0) {
            j = i * MAX_STRING_LENGTH;
            k = 0;
            b = true;
            while(courseName[j] != 0 || name[k] !=0) {
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
    bool res = false, partialRes = true;
    int i = index * MAX_STRING_LENGTH;
    char s[MAX_STRING_LENGTH];
    int si;
    si = 0;
    if(depandence[i] == 0) {
        return true;
    }
    while(1) {
        if(depandence[i] == 0 | depandence[i] == ';') {
            s[si] = 0;
            partialRes = partialRes & find(s);
            si = 0;
            res = res | partialRes;
            partialRes = true;
            if(res) {
                return true;
            }
            if(depandence[i] == 0) {
                break;
            }
        } else if(depandence[i] == ',') {
            s[si] = 0;
            partialRes = partialRes & find(s);
            si = 0;
        } else {
            s[si] = depandence[i];
            si = si + 1;
        }

        i = i + 1;
    }
    return false;
}

int main() {
    char s[MAX_STRING_LENGTH], ch;
    int i;

    // handle input
    while(1) {
        // input course name
        i = MAX_STRING_LENGTH * courseCount;
        while(1) {
            ch = getchar();
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
        ch = getchar();
        credit[courseCount] = ch - '0';
        ch = getchar();

        // input depandence
        i = MAX_STRING_LENGTH * courseCount;
        while(1) {
            ch = getchar();
            if(ch == '|' || ch == '\n') {
                break;
            }
            depandence[i] = ch;
            i = i + 1;
        } 
        depandence[i] = 0;

        // input grade
        // A = 4, B = 3, C = 2, D = 1, F = 0, none = -1
        ch = getchar();
        if(ch >= 'A' && ch <= 'F') {
            grade[courseCount] = 4 - (ch - 'A');
            if(ch == 'F') {
                grade[courseCount] = 0;
            }
            ch = getchar();
        } else {
            grade[courseCount] = -1;
        }
        courseCount += 1;
    }

    int hoursAttempted = 0;
    int hoursCompleted = 0;
    int creditsRemaining = 0;
    double gpa = 0;

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
    
    printf("GPA: %.1lf\n", gpa);
    printf("Hours Attempted: %d\n", hoursAttempted);
    printf("Hours Completed: %d\n", hoursCompleted);
    printf("Credits Remaining: %d\n", creditsRemaining);
    printf("\nPossible Courses to Take Next\n");
    if(recommendationCount > 0) {
        i = 0;
        while(i < recommendationCount) {
            printf("  %s\n", &courseName[recommendation[i] * MAX_STRING_LENGTH]);
            i = i + 1;
        }
    } else {
        if(creditsRemaining == 0) {
            printf("  None - Congratulations!\n");
        }
    }
    return 0;
}