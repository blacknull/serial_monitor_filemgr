import os
import re
import binascii
from platformio.public import DeviceMonitorFilterBase

class file_mgr(DeviceMonitorFilterBase):
    NAME = "file_mgr"

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)

        self._send_buf = ""    
        if self.options.get("eol") == "CR":
            self._eol = "\r"
        elif self.options.get("eol") == "LF":
            self._eol = "\n"
        else:
            self._eol = "\r\n"

        self._save_data = False
        self._buf = ""
        self._rcv_count = 0
        self._file_name = ""

    def __call__(self):
        if not os.path.isdir("data"):
            os.makedirs("data")

        print("%s filter is loaded" % "file_mgr")
        return self
    
    def rx(self, text):
        if not self._save_data:
            # put last line into self._buf
            line_count = text.count('\n')
            last_line = ""
            if line_count <= 0:
                last_line = text
                self._buf = self._buf + last_line
            else:
                last_line = text.split('\n', line_count)[line_count]
                self._buf = last_line
            
            if self._buf.find('[save]') < 0:
                return text
            else:
                self._save_data = True
                self._rcv_count = 0

                split_lines = self._buf.split('[save]', 1)
                self._buf = split_lines[1]
                
                if text.count('[save]') > 0:
                    return text.split('[save]', 1)[0] + '<<< begin saving data >>>\n'
                else: # text like 've]28,74,59'
                    return '...\n<<< begin saving data >>>\n'
        else:
            self._buf = self._buf + text
            if self._buf.rfind('[/file]') >= 0: # file name
                file_name = self._buf.split('[/file]', 1)[0]
                self._buf = self._buf.split('[/file]', 1)[1]
                self._file_name = file_name.split('[file]', 1)[1]
                return 'file name: %s\n' % self._file_name
                                            
            if self._buf.rfind('[/save]') >= 0: # data sending completed
                extra_str = self._buf.split('[/save]', 1)[1]
                self._buf = self._buf.split('[/save]', 1)[0]

                pattern = r'\b[0-9a-fA-F]{2}\b'
                hex_values = re.findall(pattern, self._buf)

                hex_data = binascii.unhexlify(''.join(hex_values))
                data_file = os.path.join('data', self._file_name) #"d:/receive.dat" # 
                with open(data_file, 'wb') as f:
                    f.write(hex_data)
                
                self._save_data = False
                return '[/save]' + "\n" + extra_str # self._buf + " save to " + os.path.abspath(data_file) + "\n" + extra_str # 
            else:
                if text.count(',') > 0:
                    self._rcv_count = self._rcv_count + text.count(',')                
                    return '%d bytes saving...\n' % self._rcv_count
                else:
                    return ''
        
                
    def tx(self, text):
        if self._send_buf and text == "\b":
            self._send_buf = self._send_buf[:-1]
        #    miniterm.write('test')
        else: 
            self._send_buf += text
        #print(self._send_buf)

        if self._send_buf.endswith(self._eol):
            text = self._send_buf
            self._send_buf = ""
            return text
        return ""
    
    