#include <unistd.h>
#include <stdio.h>

int main(int argc, char * argv[]){
    int val;

    while (read(3,&val,4)==4){
        printf("%d\n", val);
    }
}
