  
<p align="center">
  <img src="https://github.com/larty77/ICENet/assets/125078218/c28309e2-377a-450f-9440-8b7e8eaf335a" alt="logo" width="200" height="200">
</p>

<h1 align="center" tabindex="-1" dir="auto"><a class="anchor" aria-hidden="true"></a>ICENet C++</h1>

My convoluted implementation of a reliable protocol. If this is to be used, it should be used at most in a local network. This implementation is similar to my existing <a href = "https://github.com/larty77/ICENet/">C# lib</a>

<h2 tabindex="-1" dir="auto"><a class="anchor" aria-hidden="true"></a>About</h2>

<h3>The library now has:</h3>

<ul>
  <li style="font-size: smaller;">Installation and support connections (RUDP functionality) - !!ITS NOT TCP!!</li>
  <li style="font-size: smaller;">Handle/Send reliable packets (RUDP functionality) - !!ITS NOT TCP!!</li>
  <li style="font-size: smaller;">Convenient logging system</li>
  <li style="font-size: smaller;">Minimum thread safety system</li>
  <li style="font-size: smaller;">Ability to change the low-level transport</li>
  <li style="font-size: smaller;">The library is cross-platform</li>
</ul>

If you know enough about CMake, you can even write your own low-level transport (in the ice.sock folder). You can write your own low-level transport using a_client and a_server as absractions(Btw, I already created useful examples, they come with the library).

<h3>CMake (.lib) or (.dll) or (.so): </h3>

You may have noticed that library included folders with an ending (.lib), (.dll), (.so).. You can also use it in C#, but it little uncomfortable, so to make it easier, I created a wrap for C#, so you don’t have to work with the (.dll) and (.so) directly!

<h3>What to take: </h3>
<table>
  <tr>
    <td style="font-size: smaller;">(Windows C++)</td>
    <td style="font-size: smaller;">(.lib)</td>
    <td style="font-size: smaller;">(.dll)</td>
    <td style="font-size: smaller;">(.so)</td>
  </tr>
  <tr>
    <td style="font-size: smaller;">(Windows C#)</td>
    <td style="font-size: smaller;">  </td>
    <td style="font-size: smaller;">(.dll)</td>
    <td style="font-size: smaller;">  </td>
  </tr>
  <tr>
    <td style="font-size: smaller;">(Android C#)</td>
    <td style="font-size: smaller;">  </td>
    <td style="font-size: smaller;">  </td>
    <td style="font-size: smaller;">(.so)</td>
  </tr>
  <tr>
    <td style="font-size: smaller;">(Unity C#)</td>
    <td style="font-size: smaller;">  </td>
    <td style="font-size: smaller;">(.dll)</td>
    <td style="font-size: smaller;">(.so)</td>
  </tr>
</table>

<h3>(.lib): </h3>

<ul>
  <li style="font-size: smaller;">Everything I've added to the library is available to you.</li>
  <li style="font-size: smaller;">You have full control over the server and client fields, you can change the low-level transport.</li>
  <li style="font-size: smaller;">(.lib) libraries are very easy to use</li>
</ul>

<h3>(.dll): </h3>

<ul>
  <li style="font-size: smaller;">A separate file (ice_net.h) describes the methods you can use. Obviously, the flexibility of (.dll) is less than that of (.lib).</li>
  <li style="font-size: smaller;">The (.dll) can be used even in C#, and EVEN in Unity (Only Windows)</li>
</ul>

<h3>(.so): </h3>

<ul>
  <li style="font-size: smaller;">A separate file (ice_net.h) describes the methods you can use. Obviously, the flexibility of (.so) is less than that of (.lib).</li>
  <li style="font-size: smaller;">The (.so) can be used even in C#, and EVEN in Unity (Only Android)</li>
</ul>
