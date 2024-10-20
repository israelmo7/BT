#include <SoftwareSerial.h>
#include <Mouse.h>
#define MAX_SCREEN_SIZE(x) ((x)?(1651):3048)
#define IS_PHONE false

#define IS_PC true
#define ABS(x) ((x < 0)? x*-1: x)
#define MOVE_BY_OFFSET(x,y) (moveWithoutRest(x,y,true))
#define MOVE_BY_LOCATION(x,y) (moveWithoutRest(x,y))
#define IS_NUMBER(x) ((x >= '0' && x <= '9')? true: false)
#define IS_POSITIVE(x) ( (x>0)? 1:-1 )
#define X_SIZE 1500
#define Y_SIZE 3000
#define MAX_OFFSET 32
#define MOUSE_JUMP 127
#define MAX_OF_CHACE 3

/*    TODO:
 *     1. changing the X/Y_SIZE to the size of the device's screen
 *     2. check the Delay time variable
 *
 *
 *
 */

SoftwareSerial BT(10, 11); // rx,tx

//Segs
char __Buffer[10] = {0};
unsigned short _BufferCounter=0;
int __DataSegment[2] = { 0,0 };
int __PointerSegment[2] = { 0 };
int __CheckSum = 0;

//Structs

struct Node
{
    int _x;
    int _y;
    char* _payload;
    Node* _before;
    Node* _next;

    Node(int x, int y, char* payload, Node* before = NULL)
    {
        _x = x;
        _y = y;
        strcpy(_payload, payload);
        _next = NULL;
        _before = before;
    }
};

//Functions
void cacheFirst(char tav);
void movePointer(bool reset = true);
void resetMousePos();
Node* payloadString2NodeList(String pd);
void moveWithoutRest(int x, int y, bool offset = false);
int atoiM(char* s);
int strlenM(char* s);
bool checkValidCheckSum(char infoZero);
int reciverData();
int selectorData(char tav);
void clearRegs();
int setRegs(char tav);
void playCommands(char tav);
void semiClick();

//Payloads
String payload = "T800:500!Lc 0:-200!Oc 444:444!Lc 0:-150!O 150:0!O 0:150!Oc";
String openScreen = "T720:2555!L.c 0:-1444!O.c 380:2333! Lc0:-613!O.310:313!O310:-313!O.0:613!O.c",
dblExit = "740:2940!L.c.0:-222!Oc.740:2940!Lc.0:-222!Oc.",
settings = "150:50!Lc 0:1500!Oc 133:230!LC.780:2000!L.", //GOOD so far.
scroll = "720:2750!L.c 0: -1500!Oc.",
devsStage = scroll + scroll + scroll + scroll + scroll + scroll + scroll + ";780:2750!LC." + scroll + scroll + scroll + ";720:1613!L.C"; // need 2 times/screens
String payload_apk = openScreen + dblExit + settings + devsStage;

//Vars
unsigned int delayTimeRST = 101, delayTime = 125;
bool mouseState = false; // false for not pressed
char toPrint[50] = "";




void setup() {
    // put your setup code here, to run once:
    
    Serial.begin(9600);
    BT.begin(9600);
    Mouse.begin();
    clearRegs();
}
void loop()
{
    reciverData();


}
////////////////////////////////////////////////////////////////////


int reciverData()
{
    char tav = 0;

    if (BT.available())
    {
        tav = BT.read();
        //BT.print(">>: ");
        BT.println(tav);
    }
    else
    {
      tav=-1;
    }
//////////////////////////////////////////////

    if (tav == '{')
    {
      __CheckSum = 0;
        __Buffer[0] = 0;
        _BufferCounter=0;
    }
    else if (tav == '}')
    {
        __CheckSum = atoiM(__Buffer);
        //BT.println("CheckSum: ");
        //BT.println(__CheckSum);
        __Buffer[0] = 0;
        _BufferCounter=0;
    }
    else if(tav != -1)
    {
        selectorData(tav);
    }
    //
    //
}
int selectorData(char tav)
{

    if (IS_NUMBER(tav) || (tav == '-') || (tav == '!') || (tav == ':'))
    {
        setRegs(tav);
    }
    else if(__CheckSum)
    {
        cacheFirst(tav);
    }
    else
    {
      playCommands(tav);
    }
}
bool checkValidCheckSum(char infoZero)
{
    int ourCheckSumCalc = 0;

    ourCheckSumCalc = ABS(__DataSegment[0])+ABS(__DataSegment[1]) + int(infoZero);
    sprintf(toPrint,"Data1: %d\n",ABS(__DataSegment[1]));
    //BT.println(toPrint);
    
    return (ourCheckSumCalc == __CheckSum);
}
void clearRegs()
{

  __Buffer[0] = 0;
  _BufferCounter=0;
  __DataSegment[0] = 0;
  __DataSegment[1] = 0;

  __PointerSegment[0]=0;
  __PointerSegment[1]=0;
  
  //__CheckSum=0;
}
void cacheFirst(char tav)
{
    if(checkValidCheckSum(tav))
    {
      playCommands(tav);
    }
    else
    { 
      __CheckSum = 0;
      //clearRegs();
      //BT.print("$");
    }
}
int strlenM(char* s)
{
  int len =0;
  while(s[len]){len++;};

  return len;
}
int atoiM(char* s)
{
    int pii = 1,len= strlenM(s) , i =0;
    int result = 0;
    
    for (i=len-1; i > 0; i--)
    {
        result += ((s[i]-'0')*pii);
        pii *= 10;    
    }
    if(s[0] == '-')
    {
      result *= -1;
    }
    else
    {
      result += ((s[0]-'0') * pii);
    }

    return result;
}
int setRegs(char tav)
{
    if (tav == ':')
    {
        __DataSegment[0] = atoiM(__Buffer);//.toInt();
        
        //BT.print("__DataSegment[0]= ");
        //BT.println(__Buffer);
      
        __Buffer[0] = 0;
        _BufferCounter=0;
  
    }
    else if (tav == '!')
    {
        __DataSegment[1] = atoiM(__Buffer);//.toInt();
        //BT.println(__DataSegment[1]);
 
        //BT.print("__DataSegment[1]= ");
        //BT.println(__Buffer);
        __Buffer[0] = 0;
        _BufferCounter=0;
  
    }
    else
    {
        sprintf(toPrint,"_Buffer[%d] = %c\n_Buffer[%d] = 0\n",_BufferCounter,tav,_BufferCounter+1);
        
        //BT.println(toPrint);
        __Buffer[_BufferCounter++] = tav;
        __Buffer[_BufferCounter] = 0;

    }

}
void playCommands(char tav)
{
    sprintf(toPrint,"Play: %c\n",tav);
    BT.println(toPrint);
    switch (tav)
    {
    case '@':
        __CheckSum=0;
        __Buffer[0] = 0;
        _BufferCounter=0;
        __DataSegment[0] = 0;
        __DataSegment[1] = 0;
        mouseState = false;
        Serial.println("Cleaned!");
        break;
    case '*':
        Serial.println("Payload Mode Activated");
        tester(payload_apk);
        break;
    case 'T':
        mouseState = false;
        resetMousePos();
        break;
    case 'H':
        BT.println("A-status ||D-developer ||R-movingMode ||@-cleaning ||*-payload\n");
        break;
    case 'A':
        BT.println("Alive!");
        break;
    case '.':
        delay(delayTime);
        break;

    case 'C':

        Mouse.click(MOUSE_LEFT);
        mouseState = false;
        Serial.println("Clicked");
        break;
    case 'c':
        semiClick();
        break;
    case 'L':
        MOVE_BY_LOCATION(__DataSegment[0], __DataSegment[1]);
        break;
    case 'O':
        MOVE_BY_OFFSET(__DataSegment[0], __DataSegment[1]);
        break;
    default:
        break;    
    }
}
void semiClick()
{
    if (mouseState)
    {
        Mouse.release(MOUSE_LEFT);
        mouseState = false;
    }
    else
    {
        Mouse.press(MOUSE_LEFT);
        mouseState = true;
    }
    delay(72);
    sprintf(toPrint, "Pointer being Pressed? %d", mouseState);
    Serial.println(toPrint);
}
////////////////////////////////////////////////////////////////////

/*
 *  This Function tests
 *
 *  the program with payload
 *    Input:
 *            STRING pd - the payload
 *    Output:
 *          None
 */
void tester(String pd)
{
   
    //
}

/*
 *  This Function display the Linked List's nodes.
 *    Input:
 *            Node* head - the Linked List's head node.
 *    Output:
 *            None
 */
void displayNodes(Node* head)
{
    int i = 0;
    for (i = 1; head; i++)
    {
        sprintf(toPrint, "-NODE %d-\n[X: %d|Y: %d]\n[Payload: %s\n", i, head->_x, head->_y, head->_payload);
        //Serial.println(toPrint);
    }

}
/*
 *  This Function convert the Payload(string) to Linked List with Node object.
 *    Input:
 *            STRING pd - the payload
 *    Output:
 *            Node* - pointer to the head of the Linked List.
 */
Node* payloadString2NodeList(String pd)
{
    int i = 0, tmpValue = 0;
    int len = pd.length();
    String stmp = "";
    Node* tmp = NULL;
    Node* head = NULL;


    head = new Node(0, 0, "");

    tmp = (head);
    for (int i = 0; i < len; i++)
    {
        if (pd[i] == ':')
        {
            tmpValue = stmp.toInt();
            stmp = "";
        }
        else if (pd[i] == '!')
        {
            Node* node = new Node(tmpValue, stmp.toInt(), "");
            tmp->_next = node;
            tmp = tmp->_next;
            stmp = "";
        }
        else if (IS_NUMBER(pd[i]) || pd[i] == '-')
        {
            stmp += pd[i];
        }
    }
    return head;

}
/*
 *  This Function moves the pointer over the screen from the old place
 *    Input:
 *            INT x - x Axis
 *            INT y - y Axis
 *    Output:
 *            None
 */
void moveWithoutRest(int x, int y, bool offset = false)
{
    //movePointer(x-__PointerSegment[0], y-__PointerSegment[1], false);

/////////////////////////////////////////////
    int i = 0;
    int distanceX = (offset) ? x : x - __PointerSegment[0];
    int distanceY = (offset) ? y : y - __PointerSegment[1];
    distanceX += (__DataSegment[0] + __DataSegment[1] + x + y) % MAX_OFFSET; //Make it abit rand
    distanceY -= (__DataSegment[0] + __DataSegment[1] + x + y) % MAX_OFFSET;

    int repsX = ABS(distanceX) / MOUSE_JUMP + (((ABS(distanceX) % MOUSE_JUMP) > 0) ? 1 : 0);
    int repsY = ABS(distanceY) / MOUSE_JUMP + (((ABS(distanceY) % MOUSE_JUMP) > 0) ? 1 : 0);
    int reps = (repsY > repsX) ? repsY : repsX;

    int jumpX = distanceX / reps;
    int jumpY = distanceY / reps;

    sprintf(toPrint, "(movingINSIDE)Moving %d Times: [%d, %d]\n", reps, jumpX, jumpY);
    Serial.print(toPrint);

    for (i = 0; i < reps; i++)
    {
        Mouse.move(jumpX, jumpY, 0);
        delay(delayTimeRST);
    }
    sprintf(toPrint, "(movingEND)[%d, %d] => [%d, %d]\n", __PointerSegment[0], __PointerSegment[1], jumpX * reps, jumpY * reps);
    //sprintf(toPrint,"(reseting)[%d, %d] => [%d, %d]\n",__PointerSegment[0], __PointerSegment[1], 0, 0);

    __PointerSegment[0] += jumpX * reps;
    __PointerSegment[1] += jumpY * reps;
    __PointerSegment[0] = (__PointerSegment[0] > X_SIZE) ? X_SIZE : (__PointerSegment[0] < 0) ? 0 : __PointerSegment[0];
    __PointerSegment[1] = (__PointerSegment[1] > Y_SIZE) ? Y_SIZE : (__PointerSegment[1] < 0) ? 0 : __PointerSegment[1];
    Serial.print(toPrint);
}
/*
 *  This Function reset the pointer location on the screen by moving to the edge by few jumps
 *    Input:
 *            None
 *    Output:
 *            None
 */
void resetMousePos()
{
    int reps = MAX_SCREEN_SIZE(IS_PHONE) / MOUSE_JUMP + 1;

    for (; reps > 0; reps--)
    {
        Mouse.move(-MOUSE_JUMP, -MOUSE_JUMP, 0);

        delay(delayTimeRST);
    }
    __PointerSegment[0] = 0;
    __PointerSegment[1] = 0;
    sprintf(toPrint, "(reseting)[%d, %d] => [%d, %d]\n", __PointerSegment[0], __PointerSegment[1], 0, 0);
    Serial.print(toPrint);
}
