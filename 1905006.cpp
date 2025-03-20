// #include<iostream>
// #include<string>
// #include<vector>
// #include<algorithm>
// #include<math.h>
#include<bits/stdc++.h>
#include <windows.h>
using namespace std;

ofstream debug("debug.txt") ;
string str , generator_polynomial;
int m , r;
double probability;
vector<vector<int>> datablock , finalDataBlock_withCheckBits , finalDataBlock_withoutCheckBits;
vector<int> column_wiseSerializeddata , column_wiseSerializeddata_for_CheckSum;


//5th query
void addCheckSumBit(){
    column_wiseSerializeddata_for_CheckSum = column_wiseSerializeddata;
    for(int i = 0 ; i < generator_polynomial.size() - 1; i++){
        column_wiseSerializeddata_for_CheckSum.push_back(0);
    }

    for(int i = 0 ; i <= column_wiseSerializeddata_for_CheckSum.size() - generator_polynomial.size() ; i++){
        if(column_wiseSerializeddata_for_CheckSum[i] == 0)
            continue;
        for(int j = i ; j < i + generator_polynomial.size() ; j++){
            column_wiseSerializeddata_for_CheckSum[j]^= (generator_polynomial[j-i] - '0');
        }
    }

    for(int i = column_wiseSerializeddata.size() ; i < column_wiseSerializeddata_for_CheckSum.size() ; i++){
        column_wiseSerializeddata.push_back(column_wiseSerializeddata_for_CheckSum[i]);
    }
}

//4th query
void make_ColumnWise_Data(int size_of_padded_str){
    for(int i = 0 ; i < (m * 8 + r) ; i++){
        for(int j = 0 ; j < (size_of_padded_str / m) ; j++)
        {
            column_wiseSerializeddata.push_back(datablock[j][i]);
        }
    }
}

//3rd query
void calculate_r(){
    r = 0;
    //cout<<pow(2,r);
    while(m * 8 + r + 1 > pow(2 , r)){
        r++;
    }
    //r--;
}

bool isPowerofTwo(int n){
    while(n % 2 == 0){
        n/= 2;
    }
    if(n == 1)
        return 1;
    else
        return 0;
}

void addCheckBits(int size_of_padded_str){
    for(int i = 0 ; i < (size_of_padded_str / m) ; i++){
        for(int j = 0 ; j < r ; j++){
            int temp_bit = 1;
            temp_bit = (temp_bit << j) - 1;
            datablock[i].insert(datablock[i].begin() + temp_bit , 0);
        }

        for(int j = 0 ; j < r ; j++){
            int temp_bit = 1;
            temp_bit <<= j;
            int parity = 0;
            for(int k = temp_bit + 1 ; k <= datablock[i].size() ; k++){
                if((temp_bit & k) != 0){
                    parity^= datablock[i][k-1];
                }
            }
            datablock[i][temp_bit-1] = parity;
        }
    }
}

//2nd query
void makeDatablock(int size_of_padded_str){
    int index = -1;
    for(int i = 0 ; i < size_of_padded_str ; i++){
        if(i % m == 0){
            index++;
        }
        char c = str[i];
        int temp_bit = 1;
        vector<int> temp_vector;
        for(int j = 0 ; j < 8 ; j++){
            //temp_bit = temp_bit << j;
            //  cout<<mask<<endl;
            if((c & temp_bit) == 0){
                temp_vector.push_back(0);
            }
            else
                temp_vector.push_back(1);
            temp_bit <<= 1;
        }
        reverse(temp_vector.begin() , temp_vector.end());

        for(int j = 0 ; j < 8 ; j++){
            datablock[index].push_back(temp_vector[j]);
        }
    }
}

//1st query
void padding(){
    while(((str.size()) % m) != 0){
        str+= "~";
    }
}

int main(){
    cout<<"enter data string: ";
    getline(cin, str);

    cout<<"enter number of data bytes in a row <m>: ";
    cin>>m;

    cout<<"enter probability <p>: ";
    cin>>probability;

    cout<<"enter generator polynomial: ";
    cin>>generator_polynomial;

    padding();

    cout<<"\n\ndata string after padding: "<<str<<endl<<endl;

    int size_of_padded_str = str.size();
    datablock.resize(size_of_padded_str / m);
    finalDataBlock_withCheckBits.resize(size_of_padded_str / m);
    finalDataBlock_withoutCheckBits.resize(size_of_padded_str / m);

    makeDatablock(size_of_padded_str);

    cout<<"data block <ascii code of m characters per row>: "<<endl;

    for(int i = 0 ; i< (size_of_padded_str / m) ; i++){
        for(int j = 0 ; j < datablock[i].size() ; j++){
            cout<<datablock[i][j];
        }
        cout<<endl;
    }

    calculate_r();

    //cout<<r;



    // for(int i = 0 ; i < (size_of_padded_str / m) ; i++){
    //     for(int j = 0 ; j < datablock[i].size() ; j++){
    //         cout<<datablock[i][j];
    //     }
    //     cout<<endl;
    // }

    addCheckBits(size_of_padded_str);

    cout<<"\ndata block after adding check bits: \n";

    HANDLE h = GetStdHandle (STD_OUTPUT_HANDLE);
    WORD wOldColorAttrs;
    CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
    GetConsoleScreenBufferInfo(h, &csbiInfo);
    wOldColorAttrs = csbiInfo.wAttributes;

    for(int i = 0 ; i < (size_of_padded_str / m) ; i++){
        for(int j = 0 ; j < datablock[i].size() ; j++){
            if(isPowerofTwo(j+1)){
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
                cout<<datablock[i][j];
            }
            else{
                SetConsoleTextAttribute(h, wOldColorAttrs);
                cout<<datablock[i][j];
            }
        }
        cout<<endl;
    }
    cout<<"\ndata bits after column-wise serialization: "<<endl;

    make_ColumnWise_Data(size_of_padded_str);

    for(int i = 0 ; i < column_wiseSerializeddata.size() ; i++){
        cout<<column_wiseSerializeddata[i];
    }
    cout<<endl;

    cout<<"\ndata bits after appending CRC checksum <send frame>: "<<endl;

    addCheckSumBit();

    for(int i = 0 ; i < column_wiseSerializeddata.size() ; i++){
        if(i >= ((size_of_padded_str / m) * (m * 8 + r))){
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 3);
        }
        cout<<column_wiseSerializeddata[i];
    }
    cout<<endl;
    //Query 6

    SetConsoleTextAttribute ( h, wOldColorAttrs);
    cout<<"\nreceived frame: "<<endl;

    vector<int> position_of_flip_bits;
    std::mt19937_64 rng;
    // initialize the random number generator with time-dependent seed
    uint64_t timeSeed = chrono::high_resolution_clock::now().time_since_epoch().count();
    seed_seq ss{uint32_t(timeSeed & 0xffffffff), uint32_t(timeSeed>>32)};
    rng.seed(ss);
    // initialize a uniform distribution between 0 and 1
    uniform_real_distribution<double> unif(0, 1);
    for(int i = 0 ; i < column_wiseSerializeddata.size() ; i++){
        double currentRandomNumber = unif(rng);
        // cout<<currentRandomNumber<<endl;
        if(currentRandomNumber <= probability){
            column_wiseSerializeddata[i]^= 1;
            position_of_flip_bits.push_back(i);
        }
    }
    int posIndex = 0;
    for(int i = 0 ; i < column_wiseSerializeddata.size() ; i++){
        if((posIndex < position_of_flip_bits.size()) && (position_of_flip_bits[posIndex] == i)){
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
            posIndex++;
        }
        else{
            SetConsoleTextAttribute ( h, wOldColorAttrs);
        }
        cout<<column_wiseSerializeddata[i];
    }
    SetConsoleTextAttribute ( h, wOldColorAttrs);
    cout<<endl;

    //query 7

    cout<<"\nresult of CRC checksum matching: ";
    auto temp_column_wiseSerialData = column_wiseSerializeddata;
    for(int i = 0 ; i <= temp_column_wiseSerialData.size() - generator_polynomial.size() ; i++){
        if(temp_column_wiseSerialData[i]==0)
            continue;
        for(int j = i ; j < i + generator_polynomial.size() ; j++){
            temp_column_wiseSerialData[j]^= (generator_polynomial[j-i] - '0');
        }
    }
    bool error_flag = false;
    for(int i = 0 ; i < temp_column_wiseSerialData.size() ; i++){
        if(temp_column_wiseSerialData[i] == 1){
            error_flag = true;
            break;
        }
    }
    if(error_flag){
        cout<<"error detected"<<endl;
    }
    else{
        cout<<"no error detected"<<endl;
    }

    //query 8

    cout<<"\ndata block after removing CRC checksum bits: "<<endl;

    for(int i = 0 ; i < (size_of_padded_str / m) ; i++){
        for(int j = 0 ; j < m * 8 + r ; j++){
            finalDataBlock_withCheckBits[i].push_back(column_wiseSerializeddata[i + j * (size_of_padded_str / m)]);
        }
    }
    posIndex = 0;
    //debug<<position_of_flip_bits.size()<<" \n";
    // for(int i = 0 ; i < position_of_flip_bits.size() ; i++){
    //     debug<<position_of_flip_bits[i]<<" ";
    // }
    for(int i = 0 ; i < (size_of_padded_str / m) ; i++){
        for(int j = 0 ; j < m * 8 + r ; j++){
            auto it = find(position_of_flip_bits.begin(), position_of_flip_bits.end(), (i + j * (size_of_padded_str / m)));
            //if((posIndex < position_of_flip_bits.size()) && (position_of_flip_bits[posIndex] == (i + j * (size_of_padded_str / m)))){
            if(it != position_of_flip_bits.end()){
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 4);
                //debug<<"HI"<<endl;
                posIndex++;
            }
            else{
                SetConsoleTextAttribute ( h, wOldColorAttrs);
            }
            cout<<finalDataBlock_withCheckBits[i][j];
        }
        cout<<endl;
    }

    //query 9

    SetConsoleTextAttribute ( h, wOldColorAttrs);
    cout<<"\ndata block after removing checkbits: "<<endl;

    for(int i = 0 ; i < (size_of_padded_str / m) ; i++){
        for(int j = 0 ; j < m * 8 + r ; j++){
            if(isPowerofTwo(j+1))
                continue;
            finalDataBlock_withoutCheckBits[i].push_back(finalDataBlock_withCheckBits[i][j]);
        }
    }

    for(int i = 0 ; i < (size_of_padded_str / m) ; i++){
        for(int j = 0 ; j < finalDataBlock_withoutCheckBits[i].size() ; j++){
            cout<<finalDataBlock_withoutCheckBits[i][j];
        }
        cout<<endl;
    }

    //Error Correction
    cout<<"\noutput frame: ";

     for(int i = 0 ; i < (size_of_padded_str / m) ; i++){
        for(int j = 0 ; j < r ; j++){
            int temp_bit = 1;
            temp_bit <<= j;
            int parity = finalDataBlock_withCheckBits[i][temp_bit-1];
            for(int k = temp_bit + 1 ; k <= finalDataBlock_withCheckBits[i].size() ; k++){
                if((temp_bit & k) != 0){
                    parity^= finalDataBlock_withCheckBits[i][k-1];
                }
            }
            finalDataBlock_withCheckBits[i][temp_bit-1] = parity;
        }
    }

    for(int i = 0 ; i < (size_of_padded_str / m) ; i++){
        int wrongIndex = 0;
        bool flag = false;
        for(int j = 0 ; j < r ; j++){
            int temp_bit = 1;
            temp_bit = (temp_bit << j);
            //debug<<temp_bit<<endl;
            if(finalDataBlock_withCheckBits[i][temp_bit-1] != 0){
                wrongIndex+= temp_bit;
                //debug<<"Temp bit - "<<temp_bit<<endl;
                flag = true;
            }
        }
        if(flag){
            //debug<<wrongIndex<<endl;
           // debug<<"row - "<<i<<"column - "<<wrongIndex-1<<" value - "<<finalDataBlock_withCheckBits[i][wrongIndex-1]<<endl;
            finalDataBlock_withCheckBits[i][wrongIndex-1]^= 1;
        }
    }

    /*for(int i = 0 ; i < (size_of_padded_str / m) ; i++){
        finalDataBlock_withoutCheckBits[i].clear();
    }*/
    //finalDataBlock_withoutCheckBits.clear();

    vector<vector<int>> final_block_after_correction;
    final_block_after_correction.resize(size_of_padded_str / m);

    for(int i = 0 ; i < (size_of_padded_str / m) ; i++){
        for(int j = 0 ; j < m * 8 + r ; j++){
            if(isPowerofTwo(j+1))
                continue;
            final_block_after_correction[i].push_back(finalDataBlock_withCheckBits[i][j]);
        }
    }

    string ans="";
    for(int i = 0 ; i < (size_of_padded_str / m) ; i++){
        for(int j = 0 ; j < final_block_after_correction[i].size() ; j = j + 8){
            bitset<8> ch;
            for(int k = j ; k < j + 8 ; k++)
            {
                ch[7-k+j] = final_block_after_correction[i][k];
            }
            ans.push_back((char)ch.to_ulong());
        }
    }

    cout<<ans<<endl;

}

