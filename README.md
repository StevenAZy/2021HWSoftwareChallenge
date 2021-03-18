# 2021HWSoftwareChallenge
A repository about HuaWei Software Challenge. (Including code, doc, etc.)
## V1.1
在 baseline 的基础上，改进每日购买服务器的数量。方案如下：
1. 记录当天 add 请求中虚拟机配置需求最大的 Core 和 Memory，和 add 请求的数量 num。
2. 购买 num/2 台服务器，每台服务器的需求恰好大于等于最大 Core 和 Memory 的需求。
结果：
总成本：9760608560；迁移次数：0；耗时：1.353

## V1.2
重新考虑一边部署一边买，对于今天新买的服务器，需要编号。
1. 每次先检查已有服务器，优先部署已有的服务器；
2. 若无法部署已有服务器，则找到最适合的服务器类型，并购买一台。
> 对于新购买的服务器需要进行编号。
结果：
总成本：2921036874；迁移次数：0；耗时：7.405