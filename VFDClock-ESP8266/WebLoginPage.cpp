//
// Created by 12053 on 2024/6/20.
//
#include "WebControl.h"
/**
  rawliteral用来方便地定义和嵌入包含HTML和JavaScript代码的长字符串，而不需要对其中的特殊字符进行转义。
  HTML压缩解析：https://www.sojson.com/jshtml.html
  JS 压缩解析：https://www.uutils.com/format/js.htm
*/
void WebControl::initLoginPage(String token) {
    loginPage = String(R"rawliteral(
<html><head><title>Login</title><meta charset="UTF-8"><meta name="viewport"content="width=device-width, initial-scale=1"><style>body{font-family:Arial,sans-serif;text-align:center;padding:50px;margin:0}h1{font-size:2.0rem;margin-bottom:40px}input[type=password]{width:100%;padding:10px;font-size:1.2rem;margin:10px 0}button{width:100%;padding:20px;font-size:1.5rem;margin:10px 0}#error{color:red;font-size:1.2rem;margin-top:20px}</style><script>function sendLoginRequest(){var password=document.getElementById('password').value;var token=document.getElementById('token').value;var xhttp=new XMLHttpRequest();xhttp.onreadystatechange=function(){if(this.readyState==4&&this.status==200){if(this.responseText==='success'){window.location.href='/main?token='+token}else{document.getElementById('error').innerText='Incorrect password 🚀'+token}}};xhttp.open('POST','/login',true);xhttp.setRequestHeader('Content-type','application/x-www-form-urlencoded');xhttp.send('password='+password+'&token='+token)}</script></head>
<body><h1>Login...🚀</h1>
<p><input type="password" id="password" placeholder="Enter password"></p>
<input type="hidden" id="token" name="token" value=")rawliteral" + String(token) + R"rawliteral(">
<p><button onclick="sendLoginRequest()">Login</button></p>
<p id="error"></p></body></html>
)rawliteral");
}

void WebControl::handleLogin() {
    if (server.method() == HTTP_POST) {
        String password = server.arg("password");
        String token = server.arg("token");
        if (userTokens.find(token) != userTokens.end() && password == correctPassword) {
            userTokens[token] = true; // 更新为已登录状态
            server.send(200, "text/plain", "success");
        } else {
            server.send(200, "text/plain", "failure");
        }
    } else {
        server.send(405, "text/plain", "Method Not Allowed");
    }
}