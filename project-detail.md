<link rel="stylesheet" href="https://cdn.jsdelivr.net/npm/bootstrap-icons@1.11.3/font/bootstrap-icons.min.css">

# ELEVATE-AI <i class="bi bi-robot"></i>

<sup>Electronic Live Encouragement Via AI Text Enhancement</sup>

# ที่มาและแนวคิดการออกแบบ <i class="bi bi-info-circle-fill"></i>

ในยุคปัจจุบันที่เทคโนโลยีและการทำงานออฟฟิศเป็นส่วนหนึ่งของชีวิตประจำวัน ปัญหาสุขภาพที่เรียกว่า **Office Syndrome** ก็ได้กลายเป็นประเด็นที่ได้รับความสนใจมากขึ้น การนั่งทำงานเป็นเวลานานในสถานที่ทำงานเป็นสาเหตุหนึ่งที่ทำให้ผู้คนมีความเสี่ยงต่อ Office Syndrome ซึ่งอาการเหล่านี้สามารถทำให้เกิดความไม่สบายและมีผลต่อสุขภาพได้ เช่น ปวดหลัง ปวดคอ เป็นต้น และยังส่งผลต่อประสิทธิภาพและความเหนื่อยล้าในการทำงานอีกด้วย

หนึ่งในวิธีป้องกันอาการของ Office Syndrome คือ การลุกจากโต๊ะทำงานเพื่อขยับร่างกายเป็นช่วง ๆ แต่การจับเวลาด้วยตัวเองนั้นเป็นเรื่องที่ไม่สะดวกและมักถูกละเลยในหลายๆ ครั้งเนื่องจากความสนใจที่อยู่กับงาน เราจึงได้คิดค้นโปรเจกต์นี้ขึ้นมาเพื่อเตือนให้ผู้คนลุกขึ้นและขยับร่างกายอย่างสม่ำเสมอ โดยใช้ ESP32-S3 ร่วมกับเซ็นเซอร์ Ultrasonic เพื่อทำการจับเวลาอัตโนมัติเมื่อผู้ใช้เริ่มนั่งที่โต๊ะทำงานโดยไม่ต้องมีการจับเวลาด้วยตัวเอง และแจ้งเตือนผ่านแอปพลิเคชัน Discord เมื่อครบเวลาที่กำหนด

นอกจากการแก้ไขปัญหาออฟฟิศซินโดรมด้วยการเตือนให้ลุกขึ้นเคลื่อนไหวแล้ว การจัดสภาพแวดล้อมในที่ทำงานให้เหมาะสมก็มีบทบาทสำคัญในการเพิ่มประสิทธิภาพและลดความเหนื่อยล้าในการทำงานเช่นกัน ปัจจัยเหล่านี้รวมถึงปริมาณแสงที่เหมาะสม, อุณหภูมิ ความชื้น และปริมาณ CO<sub>2</sub> ในอากาศ ซึ่งล้วนมีผลต่อสุขภาพ สมาธิ และประสิทธิภาพในการทำงาน เราจึงต้องการที่จะวัดค่าต่าง ๆ เหล่านี้มาแสดงผลให้กับผู้ใช้ เพื่อให้สามารถปรับปรุงสภาพแวดล้อมได้ตามต้องการ นอกจากนี้ ผู้ใช้ยังสามารถให้ปัญญาประดิษฐ์ (AI) นำข้อมูลมาวิเคราะห์เพื่อประเมินและแนะนำการปรับปรุงสภาพแวดล้อมในที่ทำงานให้เหมาะสมยิ่งขึ้น

โปรเจ็คนี้มีเป้าหมายเพื่อช่วยเพิ่มคุณภาพชีวิตของผู้คนที่ทำงานนานๆ ในสถานที่ทำงาน โดยลดความเสี่ยงของ Office Syndrome อีกทั้งยังเพิ่มประสิทธิภาพและลดความเหนื่อยล้าในการทำงานของผู้ใช้อีกด้วย

<br>

# ฟังก์ชันการทำงาน <i class="bi bi-motherboard-fill"></i>

**ฟังก์ชันหลัก**

- **แจ้งเตือนให้ลุกจากโต๊ะ** : เมื่อผู้ใช้นั่งที่โต๊ะทำงาน อุปกรณ์นี้จะเริ่มจับเวลาโดยอัตโนมัติ เมื่อครบเวลาที่กำหนดระบบจะแจ้งเตือนผู้ใช้ผ่าน Discord และหน้าจอ OLED เพื่อให้ผู้ใช้ลุกขึ้นขยับร่างกาย หลังจากที่ลุกขึ้น ระบบก็จะส่งข้อความชมเชยที่ลุกขยับร่างกายและให้กำลังใจให้แก่ผู้ใช้ และเมื่อผู้ใช้กลับมานั่ง ระบบก็จะส่งข้อความต้อนรับและสรุปข้อมูลสภาพแวดล้อมการทำงานให้กับผู้ใช้ ซึ่งโดยการส่งข้อความทั้งหมดผ่าน Discord จะใช้ AI ช่วยในการทำ Text Enchantment ผ่าน `Gemini API`

<a href='https://i.postimg.cc/nLy0Z0Mf/discord.png' target='_blank'><img src='https://i.postimg.cc/nLy0Z0Mf/discord.png' border='0' alt='discord UI' style="width:100%;"/></a>

<br>

- **แสดงและวิเคราะห์ข้อมูลสภาพแวดล้อมในการทำงาน** : แสดงผลข้อมูลของอุณหภูมิ ความชื้นในอากาศ ปริมาณแสง UV Index และปริมาณ CO<sub>2</sub> ในอากาศผ่านหน้าจอ OLED และ Node-RED Dashboard และนำข้อมูลที่่ได้ไปวิเคราะห์โดยใช้ AI ผ่าน `Gemini API` จากนั้นนำผลการวิเคราห์และข้อแนะนำเกี่ยวกับสภาพแวดล้อมในการทำงานกลับมาแสดงผลให้กับผู้ใช้


<center><a href='https://i.postimg.cc/PfyWbc7J/node-red-dashboard.png' target='_blank'><img src='https://i.postimg.cc/PfyWbc7J/node-red-dashboard.png border='0' alt='Node-RED Dashboard' style="width:90%; border-radius: 1vw;"/></a></center>

<br>

**ฟังก์ชันเสริม**

- เชื่อมต่อ WiFi และ MQTT broker ใหม่โดยอัติโนมัติเมื่อการเชื่อมต่อขาดหาย

- ไฟ LED แสดงสถานะการตรวจจับการนั่งของผู้ใช้

- Standby Mode บน Sensors Module

<br>

# วิธีใช้งาน <i class="bi bi-question-circle-fill"></i>

**Sensors Module**

- ต่อ Micro USB เข้ากับ ESP32-S3 เพื่อเริ่มการทำงานโดยอัติโนมัติ  

- สามารถกดสวิตช์เพื่อสลับระหว่าง Active Mode และ Standby Mode

- ไฟ LED แสดงสถานะของบอร์ดดังนี้

<table style="margin: 0 auto; text-align: center; width: 70%; border-collapse: collapse;">
  <thead>
    <tr>
      <th style="border: 1px solid #000000; text-align: center; padding: 5px;">LED Color</th>
      <th style="border: 1px solid #000000; text-align: center; padding: 5px;">สถานะ</th>
    </tr>
  </thead>
  <tbody>
    <tr style="border: 1px solid #000000;">
      <td style="color: #4CAF50; border: 1px solid #000000; padding: 5px;">Green</td>
      <td style="border: 1px solid #000000; padding: 5px; text-align: left;">Active Mode</td>
    </tr>
    <tr style="border: 1px solid #000000;">
      <td style="color: #FDD035; border: 1px solid #000000; padding: 5px;">Yellow</td>
      <td style="border: 1px solid #000000; padding: 5px; text-align: left;">Standby Mode</td>
    </tr>
    <tr style="border: 1px solid #000000;">
      <td style="color: #E57373; border: 1px solid #000000; padding: 5px;">Red</td>
      <td style="border: 1px solid #000000; padding: 5px; text-align: left;">ไม่สามารถเชื่อมต่อ WiFi หรือ MQTT broker ได้</td>
    </tr>
  </tbody>
</table>

**Screen Module**

- ต่อ Micro USB เข้ากับ ESP32-S3 เพื่อเริ่มการทำงาน

- หน้าจอจะเข้าสู่หน้าตั้งค่า ให้ผู้ใช้วางบอร์ดไว้ในระยะที่เซนเซอร์สามารถตรวจจับการนั่งของผู้ใช้ได้ โดยหน้าจอจะขึ้นว่า Detected จากนั้นกดสวิตช์เพื่อยืนยัน

- เมื่อตั้งค่าแล้วผู้ใช้สามารถกดสวิตช์เพื่อดูข้อมูลสภาพแวดล้อมการทำงานต่าง ๆ และผลวิเคราะห์สภาพแวดล้อมได้

- เมื่อครบเวลาที่กำหนดบอร์ดจะแจ้งเตือนโดยการแสดงผลบนหน้าจอว่า "Stand up!" และกระพริบไฟ LED ทั้งสามดวงโดยผู้ใช้สามารถลุกขึ้นและกลับมานั่งได้เลย

- ไฟ LED แสดงสถานะของบอร์ดดังนี้

<table style="margin: 0 auto; text-align: center; width: 70%; border-collapse: collapse;">
  <thead>
    <tr>
      <th style="border: 1px solid #000000; text-align: center; padding: 5px;">LED Color</th>
      <th style="border: 1px solid #000000; text-align: center; padding: 5px;">สถานะ</th>
    </tr>
  </thead>
  <tbody>
    <tr style="border: 1px solid #000000;">
      <td style="color: #4CAF50; border: 1px solid #000000; padding: 5px;">Green</td>
      <td style="border: 1px solid #000000; padding: 5px; text-align: left;">ตรวจพบผู้ใช้</td>
    </tr>
    <tr style="border: 1px solid #000000;">
      <td style="color: #FDD035; border: 1px solid #000000; padding: 5px;">Yellow</td>
      <td style="border: 1px solid #000000; padding: 5px; text-align: left;">ไม่พบผู้ใช้</td>
    </tr>
    <tr style="border: 1px solid #000000;">
      <td style="color: #E57373; border: 1px solid #000000; padding: 5px;">Red</td>
      <td style="border: 1px solid #000000; padding: 5px; text-align: left;">ไม่สามารถเชื่อมต่อ WiFi หรือ MQTT broker ได้</td>
    </tr>
  </tbody>
</table>



<br>

# อุปกรณ์ที่ใช้ <i class="bi bi-box-seam-fill"></i>

1.Sensors

- [DHT11 Temperature and Humidity Sensor Module Breakout](https://th.cytron.io/p-dht11-sensor-module-breakout)
- [MQ-135 - Gas Sensor for Air Quality](https://th.cytron.io/p-mq135-air-quality-sensor-module)
- [Digital LTR390 Ultraviolet (UV) Sensor](https://th.cytron.io/p-digital-ltr390-ultraviolet-uv-sensor)
- [HC-SR04 Ultrasonic Module](https://th.cytron.io/p-5v-hc-sr04-ultrasonic-sensor)

2.OLED Display I2C IIC 0.96″ SSD1306 128x64px

3.บอร์ด ESP32-S3 2 ตัว

<br>

# หลักการทำงาน <i class="bi bi-lightbulb-fill"></i>

`ESP32-S3` ทั้ง 2 ตัวจะรับและส่งข้อมูลกันผ่าน `MQTT broker` โดย

<ul>
   <li>
      ตัวที่ 1 (Sensors Module) อ่านค่าจากเซ็นเซอร์ 3 ตัว ได้แก่ 
      <ul>
         <li> <code>DHT11</code> เซ็นเซอร์วัดอุณหภูมิและความชื้น</li>
         <li> <code> LTR390</code> เซ็นเซอร์วัด UV และความสว่าง</li>
         <li> <code>MQ-135</code> และเซ็นเซอร์วัดปริมาณ CO<sub>2</sub> ในอากาศ</li>
      </ul>
      จากนั้นส่งข้อมูลในรูปแบบ JSON เข้าใปในระบบ 
   </li>
   <br>
   <li>
      ตัวที่ 2 (Screen Module) จะดึงข้อมูลจากระบบและนำมาแสดงผลบนหน้าจอ OLED เมื่อครบเวลาที่กำหนดจะทำการแจ้งเตือนผ่านหน้าจอพร้อมกับกระพริบไฟ LED เพื่อให้ผู้ใช้ลุกขึ้น นอกจากนี้จะมี <code>HC-SR04</code> Ultrasonic sensor เพื่อตรวจจับว่านั่งอยู่ที่โต๊ะทำงานหรือไม่ และส่งข้อมูลที่ได้ในรูปแบบ JSON กลับเข้าไปที่ระบบ
   </li>
</ul>

<br>

`Node-RED` จะทำหน้าที่ในการแสดงผลค่าต่างๆ เชื่อมต่อกับ API จัดการและประมวลผลข้อมูลทั้งหมดที่ได้รับเพื่อลดภาระการทำงานของ `ESP32-S3` ซึ่งสามารถแบ่งหน้าที่ได้ดังนี้

- รับข้อมูลในรูปแบบ JSON จาก ESP32-S3 ทั้งสองตัว ทำการแยกข้อมูลออกจากกันเพื่อให้ง่ายต่อการนำไปใช้งานใน ESP32-S3 และรวมข้อมูล JSON ให้เป็น Object เดียวเพื่อนำไปใช้ในวิเคราะห์ของ AI

- ส่งผลลัพท์การวิเคราห์ที่ได้จาก AI กลับเข้าไปในระบบเพื่อให้แสดงบนจอ OLED ของ ESP32-S3 ตัวที่ 2

- ส่งข้อมูลเวลาปัจจุบันให้กับ ESP32-S3

- แสดงผลขอมูลจากค่าของเซ็นเซอร์ต่าง ๆ ที่ดึงลงมาจาก sever ลงบน Dashboard และส่งค่าเหล่านี้ไปให้ AI วิเคราะห์ผ่าน `Gemini API` และนำกลับมาแสดงผล

- เมื่อครบเวลาที่ตั้งไว้ AI จะส่งข้อความแจ้งเตือนผ่าน Discord ให้ผู้ใช้ลุกขึ้นเพื่อขยับร่างกาย ซึ่งผู้ใช้สามารถลุกได้เลยโดย Ultrasonic sensor จะตรวจจับการลุกโดยอัตโนมัติ และเมื่อกลับมานั่งก็จะเริ่มจับเวลาใหม่อีกครั้ง หากต้องการตั้งค่าระยะในการนั่งใหม่อีกครั้ง สามารถกดปุ่ม reset ที่บอร์ดได้

<center><a href='https://i.postimg.cc/X7KtV2Gb/diagram.png' target='_blank'><img src='https://i.postimg.cc/X7KtV2Gb/diagram.png' border='0' alt='all-flow' style="width:95%; border-radius: 1vw"/></a></center>

<br><br>

<center><a href='https://i.postimg.cc/ryRDNc9h/all-flow.png' target='_blank'><img src='https://i.postimg.cc/ryRDNc9h/all-flow.png' border='0' alt='all-flow' style="width:95%; border-radius: 1vw"/></a></center>

<br>

# By SEMICOLON

<sup>Searching Endlessly for Missing Indicators: Coders Overlooking Little Operator Noticed</sup>
