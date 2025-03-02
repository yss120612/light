#include "VoiceTask.h"
#include "driver/uart.h"

#define BUFFER_SIZE 100;
#define VTAG "VOICE"
void VoiceTask::setup()
{
    cmd_cnt = 0;
    uart_config_t uart_config = {
        .baud_rate = 9600,
        // .data_bits = UART_DATA_8_BITS,
        // .parity = UART_PARITY_DISABLE,
        // .stop_bits = UART_STOP_BITS_1,
        // .flow_ctrl = UART_HW_FLOWCTRL_CTS_RTS,
        // .rx_flow_ctrl_thresh = 122,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        // .source_clk = UART_SCLK_APB,
    };
    esp_err_t err;
    esp_log_level_set(VTAG, ESP_LOG_INFO);
    err = uart_driver_install(UART_NUM_2, CMD_BUF_LEN, CMD_BUF_LEN, 0, NULL, 0);
    if (err != ESP_OK)
    {
        ESP_LOGE(VTAG, "Failed UART driver install");
    }
    else
    {
        ESP_LOGI(VTAG, "Success UART driver install");
    }
    err = uart_param_config(UART_NUM_2, &uart_config);
    if (err != ESP_OK)
    {
        ESP_LOGE(VTAG, "Failed UART config param");
    }
    else
    {
        ESP_LOGI(VTAG, "Success UART config param");
    }
    err = uart_set_pin(UART_NUM_2, GPIO_NUM_17, GPIO_NUM_16, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    if (err != ESP_OK)
    {
        ESP_LOGE(VTAG, "Failed UART set pins");
    }
    else
    {
        ESP_LOGI(VTAG, "Success UART set pins");
    }
    _timer = xTimerCreate("VoiceTimer", pdMS_TO_TICKS(3500), pdFALSE, static_cast<void *>(this), onTick);
    vasya=false;
   loadGroup();
    // ESP_ERROR_CHECK(uart_driver_install(UART_NUM_2,CMD_BUF_LEN, CMD_BUF_LEN, 0, NULL, 0));
    // ESP_ERROR_CHECK(uart_param_config(UART_NUM_2, &uart_config));
    // ESP_ERROR_CHECK(uart_set_pin(UART_NUM_2,UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
}
void VoiceTask::cleanup()
{
}

void VoiceTask::loop()
{

    int len = 0;
    int pos = 0;
    int old_pos;

    memset(buffer, 0, CMD_BUF_LEN);
    len = uart_read_bytes(UART_NUM_2, buffer, CMD_BUF_LEN, pdMS_TO_TICKS(1000));
    if (len < 0)
    {
        ESP_LOGE(VTAG, "Error read uart");
    }

    if (len > 0)
    {
        do
        {
            processBuffer(buffer + pos);
            // String res = printBuff((char *)buffer + pos, buffer[pos+1]+2);
            // ESP_LOGE(VTAG, "BUFFER=%s", res.c_str());
            old_pos = pos;
            for (int j = pos; j < len - 1; j++)
            {
                if (buffer[j] == '\x0A' && buffer[j + 1] == '\xAA')
                {
                    pos = j + 1;
                }
            }
        } while (pos != old_pos);
    }
    vTaskDelay(1);
}

void VoiceTask::processBuffer(uint8_t *buff)
{
    bool valid;
    uint8_t cnt,i;
    valid = buff[0] == 0xAA && buff[1] > 1;

    String res = "";
    // lock();
    if (valid)
    {
        switch (buff[2])
        {
        case 0:
            if (buff[1] != 8)
            {
                res = "Error Read status!";
                break;
            }
            res += "Check System Settings\\n";
            res += "Trained:";
            switch (buff[3])
            {
            case 0:
                res += "untrained.\\n";
                break;
            case 1:
                res += "trained.\\n";
                break;
            case 0xff:
                res += "out of range.\\n";
                break;
            };

            res += "Baud rate:";
            switch (buff[4])
            {
            case 0:
            case 3:
                res += "9600.\\n";
                break;
            case 1:
                res += "2400.\\n";
                break;
            case 2:
                res += "4800.\\n";
                break;
            case 4:
                res += "19200.\\n";
                break;
            case 5:
                res += "38400.\\n";
                break;
            }
            res += "IO Mode:";
            switch (buff[5])
            {
            case 0:
                res += "Pulse.\\n";
                break;
            case 1:
                res += "Toggle.\\n";
                break;
            case 2:
                res += "Clear.\\n";
                break;
            case 3:
                res += "Set.\\n";
                break;
            }
            res += "Pulse Width:";
            switch (buff[6])
            {
            case 0:
                res += "10ms.\\n";
                break;
            case 1:
                res += "15ms.\\n";
                break;
            case 2:
                res += "20ms.\\n";
                break;
            case 3:
                res += "25ms.\\n";
                break;
            case 4:
                res += "30ms.\\n";
                break;
            case 5:
                res += "35ms.\\n";
                break;
            case 6:
                res += "40ms.\\n";
                break;
            case 7:
                res += "45ms.\\n";
                break;
            case 8:
                res += "50ms.\\n";
                break;
            case 9:
                res += "75ms.\\n";
                break;
            case 10:
                res += "100ms.\\n";
                break;
            case 11:
                res += "200ms.\\n";
                break;
            case 12:
                res += "300ms.\\n";
                break;
            case 13:
                res += "400ms.\\n";
                break;
            case 14:
                res += "500ms.\\n";
                break;
            case 15:
                res += "1s.\\n";
                break;
            }
            res += "Group control:";
            switch (buff[8])
            {
            case 0:
                res += "disable.\\n";
                break;
            case 1:
                res += "system.\n";
                break;
            case 2:
                res += "user.\\n";
                break;
            }

            res += "Auto load:";
            switch (buff[7])
            {
            case 0:
                res += "DISABLE.\\n";
                break;
            case 1:
                res += "ENABLE.\\n";
                break;
            default:
                res += String(buff[7], HEX);
                res += "\\n";
                break;
            }

            break;
        case 1:
            if (buff[1] != 0xD)
            {
                res += "Error check recogniser!\\n";
                break;
            }
            res += "Check recogniser!\\n";
            res += "Total commands:";
            res += String(buff[3], DEC);
            res += "\\n";
            res += "Commands:";
            for (int8_t j = 0; j < 7; j++)
            {
                res += buff[4 + j];
                if (j == 6)
                    res += "\\n";
                else
                    res += ", ";
            }
            res += "Total records:";
            res += String(buff[11], DEC);
            res += "\\nValid comm. bitmap:";
            res += String(buff[12], BIN);
            res += "\\nGroup loaded (FF-none):";
            res += String(buff[13], HEX);
            res += "\\n";
            break;

        case 2:
            res += "Check Record train status\\n";
            res += "Trained recirds:";
            cnt = buff[3];
            res += String(cnt, HEX);
            res += "\\n";
            cnt = (buff[1] - 3) / 2;
            for (int j = 0; j < cnt; j++)
            {
                if (j > 0)
                    res += ", ";
                res += "Cell ";
                res += buff[4 + j * 2];
                res += ":";
                res += (buff[4 + j * 2 + 1] == 0 ? "UT" : buff[4 + j * 2 + 1] == 1 ? "TR"
                                                                                   : "ERR");
            }
            break;
        case 3:
            res += "Signature of one Record #";
            res += String(buff[3], DEC);
            res += " Length=";
            cnt = buff[4];
            res += String(cnt, DEC);
            if (cnt > 0)
            {
                res += " ";
                buff[cnt + 5] = 0;
                res += (char *)buff + 5;
            }
            // res+="\\n";

            // buff[cnt + 5] = 0;
            // // for (int j=0;j<cnt;j++){
            // // }
            break;
        case '\x10':
            if (buff[3] == 0)
            {
                res += "System Settings restored!\\n";
            }
            break;
        case '\x12':
            res += "Mode updated.\\n";
            // erial.println("Mode updated.");
            //  switch (buff[3]){
            //      case 0:
            //      break;
            //      case 1:
            //      break;
            //      case 2:
            //      break;
            //      case 3:
            //      break;
            //  }
            break;
        case '\x13':
            res += "Pulse period updated.\\n";
            break;
        case '\x15':
            /*
            Set Power On Auto Load (15)
            Use this command to enable or disable "Power On Auto Load" function.
            | AA| 05+n | 15 | 00 |BITMAP | R0 | ... | Rn | 0A | (set auto load)

            BITMAP Record bitmap:
            ● 00 -- zero record, disable auto load
            ● 01 -- one record
            ● 03 -- two records
            ● 07 -- three records
            ● 0F -- four records
            ● 1F -- five records
            ● 3F -- six record
            ● 7F -- seven records
            R0~Rn Record
            */
            if (buff[4] == 0)
            {
                res += "AutoLoad DISABLED\\n";
            }
            else
            {
                res += "AutoLoad ENABLED\\n";
                for (int i = 0; i < buff[1] - 4; i++)
                {
                    res += String(i, DEC);
                    res += ":";
                    res += String(buff[5 + i], DEC);
                    res += "\\n";
                }
            }
            break;

        case '\x0A':
            switch (buff[5])
            {
            case 'p':
                switch (buff[10])
                {
                case 'n':
                    res += "Speak now\\n";
                    break;
                case 'a':
                    res += "Speak again\\n";
                    break;
                }
                break;
            case 'u':
                res += "Record ";
                res += String(buff[3], DEC);
                res += " Success!\\n";
                break;
            case 'a':
                res += "Cann’t matched\\n";
                break;
            }
            break;
        case '\x20':
        case '\x21':
            res += String(buff[3], DEC);
            res += " success records trained.\\n";
            cnt = buff[2] == '\x21' ? 1 : (buff[1] - 3) / 2;
            for (int j = 0; j < cnt; j += 2)
            {
                if (j > 0)
                    res += ", ";
                res += "Cell N:";
                res += String(buff[j + 4], DEC);
                res += "[";
                switch (buff[j + 5])
                {
                case 0:
                    res += "success";
                    break;
                case 1:
                    res += "timeout";
                    break;
                case 2:
                    res += "incorrect N";
                    break;
                }
                res += "]";
            }
            if (buff[2] == '\x21')
            {
                res += " (";
                res += String((const char *)(buff + 6), buff[1] - 5);
                res += ")";
            }
            res += "\\n";

            break;
        case '\x30':
            /*
            | AA| 3+2n | 30 | N | R0 | STA0 | ... | Rn | STAn | 0A |
            R0~Rn Voice Record index
            STA0~STAn Load result
                ● 00 -- Success
                ● FF -- Record value out of range
                ● FE -- Record untrained
                ● FD -- Recognizer full
                ● FC -- Record already in recognizer
                N Number of successful training voice commands
            */
            res += "Records processed:";
            res += buff[3];
            res += "\\n";
            for (int i = 0; i < buff[3]; i++)
            {
                res += "Record ";
                res += buff[4 + i * 2];
                res += ":";
                res += String(buff[5 + i * 2], HEX);
                res += "\\n";
            }
            break;
        case '\x31':
            res += "Recogniser cleared!\\n";
            break;
        case '\x32':

            if (buff[1] == 3)
            {
                res += "Group created\\n";
            }
            else if (buff[1]==0x0D) {
                res += "Load group ";
                res += buff[3];
                res += "\\n";
                for (i=0;i<7;i++){
                    if(i>0) res+= ", ";
                    res+=String(buff[i+4],HEX);
                }
                res+="\\nTotal commands:";
                res+=String(buff[11],DEC);
                res+="\\n";
            }else if (buff[1]==0x0A) {
                res += "Group ";
                res += buff[3];
                res += "\\n";
                for (i=0;i<7;i++){
                    if(i>0) res+= ", ";
                    res+=String(buff[i+4],HEX);
                }
                res+="\\n";
            }

            break;
        case '\xFF':
            /*
            | AA | 03 | FF | ECODE | 0A |
            ECODE error code
                ● FF -- command undefined
                ● FE -- command length error
                ● FD -- data error
                ● FC -- subcommand error
                ● FB -- command usage error
            */
            res += "ERROR: ";
            switch (buff[3])
            {
            case 0xFB:
                res += "command usage error\\n";
                break;
            case 0xFC:
                res += "subcommand error\\n";
                break;
            case 0xFD:
                res += "data error\\n";
                break;
            case 0xFE:
                res += "command length error\\n";
                break;
            case 0xFF:
                res += "command undefined\\n";
                break;
            default:
                res += "other error\\n";
                break;
            }
            break;
        case 0x0D:
            /*
            Voice Recognized (0D)
            | AA | 07 | 0D | 00 | GRPM | R | RI | SIGLEN | SIG | 0A |
             GRPM Group mode indicate
                 ● FF: not in group mode
                 ● 00~0A: system group mode
                 ● 80~87: user group mode
             R record which is recognized.
             RI recognizer index value for recognized record.
             SIGLEN signature length of the recognized record, 0 means on signature, on SIG area
             SIG signature content
             */
            res += "Group :";
            res += String(buff[4], HEX);
            res += "(FF no group, 00..0A system, 80..87 user) \\n";
            res += "Record which is recognized:";
            res += String(buff[5], HEX);
            res += "\\n";
            res += "Recognizer index:";
            res += String(buff[6], HEX);
            res += "\\n";
            res += "Description:";

            if (buff[7] > 0)
            {
                buff[8 + buff[7]] = 0;
                res += (char *)buff + 8;
                res += "\\n";
            }
            else
            {
                res += "NONE\\n";
            }
            if (vasya && buff[5] >= 0x10 && buff[5] <= 0x15)
            {
               
                //loadSecond();
                makeCommand(buff[5]);
            }
            else if (buff[5]==0x16)
            {
                vasya=true;
                xTimerStart(_timer,0);
                //makeCommand(buff[5]);
            }
            break;
        default:
            res += "Other valid command!\\n";
            res += printBuff((char *)buff, buff[1] + 2);
            break;
        }
    }
    else
    {
        res += "Invalid command!\\n";
        res += printBuff((char *)buff, buff[1] + 2);
    }
    // unlock();
    if (res.length() > 0)
    {
        xMessageBufferSend(message, res.c_str(), res.length() < CMD_BUF_LEN ? res.length() : CMD_BUF_LEN, 0);
    }
}

String VoiceTask::printBuff(char *buff, uint16_t length)
{
    String r;
    r = "Length=";
    r += length;
    r += " [";
    for (int j = 0; j < length; j++)
    {
        if ((buff[j]) <= 16)
        {
            r += '0';
        }
        r += String(buff[j], HEX);
        if (j < length - 1)
            r += "|";
        else
            r += "]\n";
    }
    return r;
}

void VoiceTask::loadGroup()
{
    u_int8_t a[20];
    String v = "32:03:00";//"30:20:21:22:23:24";
    memset(a, 0, sizeof(a));
    make_command(a, ':', '+', v);
    int i = uart_write_bytes(UART_NUM_2, (const char *)a, a[1] + 2);
    xTimerStart(_timer, 0);
}

void VoiceTask::onMyTick()
{ // restore voice by default
    vasya=false;
}

void VoiceTask::onTick(TimerHandle_t tm)
{
    VoiceTask *vt = static_cast<VoiceTask *>(pvTimerGetTimerID(tm));
    if (vt)
        vt->onMyTick();
}

// void VoiceTask::makeCommand(uint8_t command)
// {
//     event_t ev;
//     switch (first_command)
//     {
//     case 0x10:
//         switch (command)
//         {
//         case 0x21:
//             ev.state = PULT_BUTTON;
//             ev.button = PULT_9;
//             xQueueSend(que, &ev, 0);
//             break;
//         case 0x22:
//             ev.state = PULT_BUTTON;
//             ev.button = PULT_7;
//             xQueueSend(que, &ev, 0);
//             break;
//         case 0x23:
//             ev.state = PULT_BUTTON;
//             ev.button = PULT_6;
//             xQueueSend(que, &ev, 0);
//             break;
//         case 0x24:
//             ev.state = PULT_BUTTON;
//             ev.button = PULT_5;
//             xQueueSend(que, &ev, 0);
//             break;
//         }
//         break;
//     case 0x11:
//         switch (command)
//         {
//         case 0x20:
//             ev.state = WEB_EVENT;
//             ev.button = RELAYSET1;
//             ev.count = 1;
//             xQueueSend(que, &ev, 0);
//             break;
//         case 0x21:
//             ev.state = WEB_EVENT;
//             ev.button = RELAYSET1;
//             ev.count = 0;
//             xQueueSend(que, &ev, 0);
//             break;
//         }
//         break;
//     case 0x12:
//         switch (command)
//         {
//         case 0x20:
//             ev.state = WEB_EVENT;
//             ev.button = RELAYSET4;
//             ev.count = 1;
//             xQueueSend(que, &ev, 0);
//             break;
//         case 0x21:
//             ev.state = WEB_EVENT;
//             ev.button = RELAYSET4;
//             ev.count = 0;
//             xQueueSend(que, &ev, 0);
//             break;
//         }
//         break;
//     case 0x13:
//         switch (command)
//         {
//         case 0x21:
//             ev.state = PULT_BUTTON;
//             ev.button = PULT_8;
//             xQueueSend(que, &ev, 0);
//             break;
//         }
//         break;
//     }
// }

void VoiceTask::makeCommand(uint8_t command)
{
    event_t ev;
    switch (command)
    {
    case 0x10:
            ev.state = PULT_BUTTON;
            ev.button = PULT_7;
            xQueueSend(que, &ev, 0);
            break;
    case 0x11:
            ev.state = PULT_BUTTON;
            ev.button = PULT_1;
            xQueueSend(que, &ev, 0);
            break;
    case 0x12:
            ev.state = PULT_BUTTON;
            ev.button = PULT_4;
            xQueueSend(que, &ev, 0);
            break;
     case 0x13:
            ev.state = PULT_BUTTON;
            ev.button = PULT_9;
            xQueueSend(que, &ev, 0);
            vTaskDelay(pdMS_TO_TICKS(1000));
            ev.button = PULT_8;
            xQueueSend(que, &ev, 0);
            break;
        case 0x14:
            ev.state = PULT_BUTTON;
            ev.button = PULT_9;
            xQueueSend(que, &ev, 0);
            break;
        case 0x15:
            ev.state = PULT_BUTTON;
            ev.button = PULT_5;
            xQueueSend(que, &ev, 0);
            break;    
       
    }
}