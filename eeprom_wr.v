module eeprom_wr(rst,sclk,w_r,addr,data,sda,scl,ack);
  input rst,sclk,w_r;
  input[10:0] addr;
  input[7:0] data;
  inout sda;
  output scl,ack;
  reg wr_buf,wr_link,high_link,low_link,scl,ack;
  reg[10:0] addr_buf,addr_link;
  reg[7:0] write_buf,write_link,read_buf;
  reg[5:0] state;
  
  parameter ready  =  6'b000000,
            wr     =  6'b000001,
            addr0  =  6'b000010,
            addr1  =  6'b000011,
            addr2  =  6'b000100,
            addr3  =  6'b000101,
            addr4  =  6'b000110,
            addr5  =  6'b000111,
            addr6  =  6'b001000,
            addr7  =  6'b001001,
            addr8  =  6'b001010,
            addr9  =  6'b001011,
            addr10 =  6'b001100,
            w0     =  6'b001101,
            w1     =  6'b001110,
            w2     =  6'b001111,
            w3     =  6'b010000,
            w4     =  6'b010001,
            w5     =  6'b010010,
            w6     =  6'b010011,
            w7     =  6'b010100,
            switch =  6'b010101,
            r0     =  6'b010110,
            r1     =  6'b010111,
            r2     =  6'b011000,
            r3     =  6'b011001,
            r4     =  6'b011010,
            r5     =  6'b011011,
            r6     =  6'b011100,
            r7     =  6'b011101,
            idle   =  6'b011110,
            finish =  6'b011111,
            termi  =  6'b100000;
            
  assign sda=high_link?1:1'bz;
  assign sda=low_link?0:1'bz;
  assign sda=wr_link?wr_buf:1'bz;
  assign sda=addr_link[0]?addr_buf[0]:1'bz;
  assign sda=addr_link[1]?addr_buf[1]:1'bz;
  assign sda=addr_link[2]?addr_buf[2]:1'bz;
  assign sda=addr_link[3]?addr_buf[3]:1'bz;
  assign sda=addr_link[4]?addr_buf[4]:1'bz;
  assign sda=addr_link[5]?addr_buf[5]:1'bz;
  assign sda=addr_link[6]?addr_buf[6]:1'bz;
  assign sda=addr_link[7]?addr_buf[7]:1'bz;
  assign sda=addr_link[8]?addr_buf[8]:1'bz;
  assign sda=addr_link[9]?addr_buf[9]:1'bz;
  assign sda=addr_link[10]?addr_buf[10]:1'bz;
  assign sda=write_link[0]?write_buf[0]:1'bz;
  assign sda=write_link[1]?write_buf[1]:1'bz;
  assign sda=write_link[2]?write_buf[2]:1'bz;
  assign sda=write_link[3]?write_buf[3]:1'bz;
  assign sda=write_link[4]?write_buf[4]:1'bz;
  assign sda=write_link[5]?write_buf[5]:1'bz;
  assign sda=write_link[6]?write_buf[6]:1'bz;
  assign sda=write_link[7]?write_buf[7]:1'bz;
  
  always@(negedge sclk or negedge rst)
  if(!rst)
    scl<=1;
  else
    scl<=~scl;
    
  always@(posedge sclk or negedge rst)
  if(!rst)
    begin
      ack<=0;
      wr_link<=0;
      high_link<=1;
      low_link<=0;
      addr_link<=11'b00000000000;
      write_link<=8'b00000000;
      state<=ready;
    end
  else
    case(state)
      ready: if(ack)
               begin
                 if(scl)
                   begin
                     high_link<=0;
                     low_link<=1;
                     wr_buf<=w_r;
                     write_buf<=data;
                     addr_buf<=addr;
                     state<=wr;
                   end
               end
             else
               ack<=1;
         wr: if(!scl)
               begin
                 low_link<=0;
                 wr_link<=1;
                 ack<=0;
                 state<=addr0;
               end
      addr0: if(!scl)
               begin
                 wr_link<=0;
                 addr_link[0]<=1;
                 state<=addr1;
               end
      addr1: if(!scl)
               begin
                 addr_link[0]<=0;
                 addr_link[1]<=1;
                 state<=addr2;
               end
      addr2: if(!scl)
               begin
                 addr_link[1]<=0;
                 addr_link[2]<=1;
                 state<=addr3;
               end
      addr3: if(!scl)
               begin
                 addr_link[2]<=0;
                 addr_link[3]<=1;
                 state<=addr4;
               end
      addr4: if(!scl)
               begin
                 addr_link[3]<=0;
                 addr_link[4]<=1;
                 state<=addr5;
               end
      addr5: if(!scl)
               begin
                 addr_link[4]<=0;
                 addr_link[5]<=1;
                 state<=addr6;
               end
      addr6: if(!scl)
               begin
                 addr_link[5]<=0;
                 addr_link[6]<=1;
                 state<=addr7;
               end
      addr7: if(!scl)
               begin
                 addr_link[6]<=0;
                 addr_link[7]<=1;
                 state<=addr8;
               end
      addr8: if(!scl)
               begin
                 addr_link[7]<=0;
                 addr_link[8]<=1;
                 state<=addr9;
               end
      addr9: if(!scl)
               begin
                 addr_link[8]<=0;
                 addr_link[9]<=1;
                 state<=addr10;
               end
     addr10: if(!scl)
               begin
                 addr_link[9]<=0;
                 addr_link[10]<=1;
                 if(wr_buf)
                   state<=switch;
                 else
                   state<=w0;
               end
          w0: if(!scl)
                begin
                  addr_link[10]<=0;
                  write_link[0]<=1;
                  state<=w1;
                end
          w1: if(!scl)
                begin
                  write_link[0]<=0;
                  write_link[1]<=1;
                  state<=w2;
                end
          w2: if(!scl)
                begin
                  write_link[1]<=0;
                  write_link[2]<=1;
                  state<=w3;
                end
          w3: if(!scl)
                begin
                  write_link[2]<=0;
                  write_link[3]<=1;
                  state<=w4;
                end
          w4: if(!scl)
                begin
                  write_link[3]<=0;
                  write_link[4]<=1;
                  state<=w5;
                end
          w5: if(!scl)
                begin
                  write_link[4]<=0;
                  write_link[5]<=1;
                  state<=w6;
                end
          w6: if(!scl)
                begin
                  write_link[5]<=0;
                  write_link[6]<=1;
                  state<=w7;
                end
          w7: if(!scl)
                begin
                  write_link[6]<=0;
                  write_link[7]<=1;
                  state<=idle;
                end
      switch: if(!scl)
                begin
                  addr_link[10]<=0;
                  state<=r0;
                end
          r0: if(!scl)
                begin
                  read_buf[0]<=sda;
                  state<=r1;
                end
          r1: if(!scl)
                begin
                  read_buf[1]<=sda;
                  state<=r2;
                end
          r2: if(!scl)
                begin
                  read_buf[2]<=sda;
                  state<=r3;
                end
          r3: if(!scl)
                begin
                  read_buf[3]<=sda;
                  state<=r4;
                end
          r4: if(!scl)
                begin
                  read_buf[4]<=sda;
                  state<=r5;
                end
          r5: if(!scl)
                begin
                  read_buf[5]<=sda;
                  state<=r6;
                end
          r6: if(!scl)
                begin
                  read_buf[6]<=sda;
                  state<=r7;
                end
          r7: if(!scl)
                begin
                  read_buf[7]<=sda;
                  state<=finish;
                end
        idle: if(!scl)
                state<=finish;
      finish: if(!scl)
                begin
                  if(!wr_buf)
                    write_link[7]<=0;
                  low_link<=1;
                  state<=termi;
                end
       termi: if(scl)
                begin
                  low_link<=0;
                  high_link<=1;
                  state<=ready;
                end
    endcase
endmodule




