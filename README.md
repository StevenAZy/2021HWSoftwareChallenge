# 2021HWSoftwareChallenge
A repository about HuaWei Software Challenge. (Including code, doc, etc.)
## V1.0
在 baseline 的基础上，改进每日购买服务器的数量。方案如下：
1. 记录当天 add 请求中虚拟机配置需求最大的 Core 和 Memory，和 add 请求的数量 num。
2. 购买 num/2 台服务器，每台服务器的需求恰好大于等于最大 Core 和 Memory 的需求。
