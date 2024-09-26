#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <fcntl.h> 
#include <unistd.h>

struct num_people
{
    int child;
    int adult;
    int elder;
};

int main()
{
    /* file device */
    int fd_led;
    fd_led = open("/dev/led_device", O_WRONLY);
    if (fd_led < 0) {
        printf("Failed to open led device\n");
        return -1;
    }

    /* initialize write device mode */
    char mode_led[9], temp_mode[9], light_on[9], light_off[9];
    strcpy(mode_led, "00000000");
    
    /* initialize numbers of sub_people in each area */
    struct num_people stadium = {0, 0, 0};
    struct num_people city = {0, 0, 0};
    struct num_people zoo = {0, 0, 0};
    // printw("%d", stadium.child);

    /* total waiting people in each area */
    int num_stadium = stadium.child + stadium.adult + stadium.elder;
    int num_city = city.child + city.adult + city.elder;
    int num_zoo = zoo.child + zoo.adult + zoo.elder;
    int num_total = num_stadium + num_city + num_zoo;
    char charValue0, charValue1, charValue2;
    int is_large = 0;

    char input; //keyboard input
    char screen[] = "Menu"; //screen mode
    char area, add_or_reduce, group, exit_or_cont; //Report mode
    int number; //Report people number(add or reduce)

    int info_child, info_adult, info_elder; //people info in one area

    while(1)
    {
        /* Program Menu */
        if(!strcmp(screen, "Menu")){
            /* enable leds */
            mode_led[0] = '1';
            mode_led[2] = '1';
            mode_led[4] = '1';

            /* close 7-seg */
            mode_led[6] = '0';
            write(fd_led, mode_led, sizeof(mode_led));
            
            printf("1. Search \n2. Report \n3. Exit \n\n");
            scanf(" %c",&input);
            switch(input)
            {
                case '1':
                    strcpy(screen, "Sear");
                    break;
                case '2':
                    strcpy(screen, "Repo");
                    break;
                case '3':
                    strcpy(screen, "Exit");
                    break;
                default:
                    break;
            }
        }
        /* Search Mode */
        else if(!strcmp(screen, "Sear")){
            printf("1. Baseball Stadium : %d \n2. Big City : %d \n3. Zoo : %d \n\n", num_stadium, num_city, num_zoo);
            
            /* if there are people, LED turns on*/
            if(num_stadium > 0) 
                mode_led[1] = '1';
            else
                mode_led[1] = '0';
            if(num_city > 0)
                mode_led[3] = '1';
            else
                mode_led[3] = '0';
            if(num_zoo > 0)
                mode_led[5] = '1';
            else
                mode_led[5] = '0';
            write(fd_led, mode_led, sizeof(mode_led));
            
            /* 7-seg total number */
            mode_led[6] = '1';
            if(num_total >= 10){ // twinkle
                mode_led[7] = num_total/100 + '0';
                write(fd_led, mode_led, sizeof(mode_led));
                usleep(1000000);
                mode_led[7] = num_total/10%10 + '0';
                write(fd_led, mode_led, sizeof(mode_led));
                usleep(1000000);
                mode_led[7] = num_total%10 + '0';
                write(fd_led, mode_led, sizeof(mode_led));
                usleep(1000000);
                mode_led[6] = '0';
            }
            else{
                mode_led[7] = num_total + '0';
                write(fd_led, mode_led, sizeof(mode_led));
                usleep(1000000);
            }
            write(fd_led, mode_led, sizeof(mode_led));
            
            scanf(" %c",&input);
            switch(input)
            {
                case '1':
                    strcpy(light_on, "11101010");
                    strcpy(light_off, "10101010");
                    if(num_stadium >= 10){ // twinkle
                        charValue0 = num_city/100 + '0';
                        charValue1 = num_stadium/10%10 + '0';
                        charValue2 = num_stadium%10 + '0';
                        is_large = 1;
                    }
                    else{
                        charValue2 = num_stadium%10 + '0';
                        is_large = 0;
                    }
                    strcpy(screen, "Info");
                    info_child = stadium.child;
                    info_adult = stadium.adult;
                    info_elder = stadium.elder;
                    break;
                case '2':
                    strcpy(light_on, "10111010");
                    strcpy(light_off, "10101010");
                    if(num_city >= 10){ // twinkle
                        charValue0 = num_city/100 + '0';
                        charValue1 = num_city/10%10 + '0';
                        charValue2 = num_city%10 + '0';
                        is_large = 1;
                    }
                    else{
                        charValue2 = num_city%10 + '0';
                        is_large = 0;
                    }
                    strcpy(screen, "Info");
                    info_child = city.child;
                    info_adult = city.adult;
                    info_elder = city.elder;
                    break;
                case '3':
                    strcpy(light_on, "10101110");
                    strcpy(light_off, "10101010");
                    if(num_zoo >= 10){ // twinkle
                        charValue0 = num_city/100 + '0';
                        charValue1 = num_zoo/10%10 + '0';
                        charValue2 = num_zoo%10 + '0';
                        is_large = 1;
                    }
                    else{
                        charValue2 = num_zoo%10 + '0';
                        is_large = 0;
                    }
                    strcpy(screen, "Info");
                    info_child = zoo.child;
                    info_adult = zoo.adult;
                    info_elder = zoo.elder;
                    break;
                case 'q':
                    strcpy(screen, "Menu");
                    break;
                default:
                    break;
            }
        }
        /* Report Mode */
        else if(!strcmp(screen, "Repo")){
            /* setting */
            printf("Area (1~3) : ");
            scanf(" %c",&area);
            printf("Add or Reduce ('a' or 'r') : ");
            scanf(" %c",&add_or_reduce);
            printf("Age group ('c', 'a', 'e') : ");
            scanf(" %c",&group);
            printf("The number of passenger : ");
            scanf(" %d",&number);
            printf("\n\n(Exit or Continue)\n\n");

            /* handle the number */
            switch(area)
            {
                //stadium
                case '1': 
                    if(group == 'c'){
                        stadium.child += ((add_or_reduce == 'a') ? (number) : (-number));
                    }
                    else if(group == 'a')
                        stadium.adult += ((add_or_reduce == 'a') ? (number) : (-number));
                    else if(group == 'e')
                        stadium.elder += ((add_or_reduce == 'a') ? (number) : (-number));
                    break;
                //city
                case '2':
                    if(group == 'c')
                        city.child += ((add_or_reduce == 'a') ? (number) : (-number));
                    else if(group == 'a')
                        city.adult += ((add_or_reduce == 'a') ? (number) : (-number));
                    else if(group == 'e')
                        city.elder += ((add_or_reduce == 'a') ? (number) : (-number));
                    break;
                //zoo
                case '3':
                    if(group == 'c')
                        zoo.child += ((add_or_reduce == 'a') ? (number) : (-number));
                    else if(group == 'a')
                        zoo.adult += ((add_or_reduce == 'a') ? (number) : (-number));
                    else if(group == 'e')
                        zoo.elder += ((add_or_reduce == 'a') ? (number) : (-number));
                    break;
                default:
                    break;
            }
            num_stadium = stadium.child + stadium.adult + stadium.elder;
            num_city = city.child + city.adult + city.elder;
            num_zoo = zoo.child + zoo.adult + zoo.elder;
            num_total = num_stadium + num_city + num_zoo;

            /* update leds*/
            if(num_stadium > 0) 
                mode_led[1] = '1';
            else
                mode_led[1] = '0';
            if(num_city > 0)
                mode_led[3] = '1';
            else
                mode_led[3] = '0';
            if(num_zoo > 0)
                mode_led[5] = '1';
            else
                mode_led[5] = '0';
            write(fd_led, mode_led, sizeof(mode_led));

            /* update 7-seg*/
            mode_led[6] = '1';
            if(num_total >= 10){ // twinkle
                mode_led[7] = num_total/100 + '0';
                write(fd_led, mode_led, sizeof(mode_led));
                usleep(1000000);
                mode_led[7] = num_total/10%10 + '0';
                write(fd_led, mode_led, sizeof(mode_led));
                usleep(1000000);
                mode_led[7] = num_total%10 + '0';
                write(fd_led, mode_led, sizeof(mode_led));
                usleep(1000000);
                mode_led[6] = '0';
            }
            else{
                mode_led[7] = num_total + '0';
                write(fd_led, mode_led, sizeof(mode_led));
                usleep(1000000);
            }
            write(fd_led, mode_led, sizeof(mode_led));

            /* Exit or Continue */
            scanf(" %c",&exit_or_cont);
            if(exit_or_cont == 'e')
                strcpy(screen, "Menu");
            else if(exit_or_cont == 'c')
                strcpy(screen, "Repo");
        }
        /* Exit Mode */
        else if(!strcmp(screen, "Exit")){
            /* close all leds and 7-seg*/
            mode_led[0] = '1';
            mode_led[1] = '0';
            mode_led[2] = '1';
            mode_led[3] = '0';
            mode_led[4] = '1';
            mode_led[5] = '0';
            mode_led[6] = '0';
            write(fd_led, mode_led, sizeof(mode_led));
            
            break;
        }
        /* Sub_info in One Area */
        else if(!strcmp(screen, "Info")){
            printf("Child : %d \nAdult : %d \nElder : %d \n\n", info_child, info_adult, info_elder);
            
            /* led and 7-seg twinkle */
            mode_led[6] = '1';
            for(int i=0;i<3;i++)
            {
                if(is_large == 1){
                    light_on[7] = charValue0;
                    light_off[7] = charValue0;
                    is_large++;
                }
                else if(is_large == 2){
                    light_on[7] = charValue1;
                    light_off[7] = charValue1;
                    is_large++;
                }
                else{
                    light_on[7] = charValue2;
                    light_off[7] = charValue2;
                }
                write(fd_led, light_on, sizeof(light_on));
                usleep(500000);
                write(fd_led, light_off, sizeof(light_off));
                usleep(500000);
            }
            light_on[6] = '0';
            write(fd_led, light_on, sizeof(light_on));
            
            system("read -p '' var");
            strcpy(screen, "Sear");
        }
    }
    return 0;
}