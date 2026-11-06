# Archive index

历史批量渲染、固件和原始记录已从docs抽离到backups/history-outputs，以下路径相对于项目根目录。所有压缩包已逐项解压验证SHA-256，Git只保存清单，不保存这些私有包。

- backups/history-outputs/renders-batch-02-v0.2.0.zip：162个文件，3.6 MiB，historical-software-rendering；SHA-256 `40ee3ac4c016626f9dcdb842fb84a1b217ecb4f7cceac1c1d848780265e1572f`。
- backups/history-outputs/renders-batch-03-v0.3.0.zip：236个文件，5.7 MiB，historical-software-rendering；SHA-256 `846be10ecc42e023198d6dd2ae58c696c7492ca2e6bf8929c22b279c2fb5eeb4`。
- backups/history-outputs/renders-device-polish-0.3.8.zip：49个文件，5.0 MiB，historical-software-rendering；SHA-256 `ea9dceb1f551db7e402690fe4dcac6b9ad6877e0b30005e3e5a8505abaefdcbf`。
- backups/history-outputs/firmware-history.zip：69个文件，21.5 MiB，firmware-and-logs；SHA-256 `f2046184fcd8f8fa360ee8273f4003839174184c3bbd02f8f2351b5e9340a624`。
- backups/history-outputs/renders-first-batch-0.3.3.zip：630个文件，17.1 MiB，historical-software-rendering；SHA-256 `902a63dcf18017648503356e545de9630adc6bb3ced0d3185ee087f89aa9fc74`。
- backups/history-outputs/renders-second-batch-0.3.4.zip：786个文件，26.1 MiB，historical-software-rendering；SHA-256 `52bb367c366079f97cdf2443e63821cde512d591205502330420c59b8b80496f`。
- backups/history-outputs/renders-settings-picker-v0.3.2.zip：461个文件，12.3 MiB，historical-software-rendering；SHA-256 `c3acdd0ee177f73525082685414d725692ad49a6015ae8327dac37f535d64471`。
- backups/history-outputs/renders-third-batch-0.3.5.zip：1038个文件，38.4 MiB，historical-software-rendering；SHA-256 `11044e7a10abef336177f0278d5929922306ba870f29d179ff216eb7f00c9929`。
- backups/history-outputs/renders-ui-polish-0.3.6.zip：1662个文件，56.8 MiB，historical-software-rendering；SHA-256 `bec3fcd8770210399e89b0b3b24be6fed6597552c1cd46f7247892098b19e4ed`。
- backups/history-outputs/renders-ui-repair-v0.3.1.zip：316个文件，8.5 MiB，historical-software-rendering；SHA-256 `692c41816d9066fe8a060e6aed5340ec2b2a7eaf244494969134d4badd371dca`。
- backups/history-outputs/renders-v0.3.9-current.zip：1668个文件，49.8 MiB，software-rendering；SHA-256 `ea622f5928974270116c28e1104b69f2212579ea1b17dbaa271a59d8a240eb63`。
- backups/history-outputs/source-records.zip：144个文件，4.7 MiB，previous-documents-indexes-and-encoded-resources；SHA-256 `5aeedfbd118e9b54ec58d387b5a3b1dbbc13cec87ed5067d92886cdbb6510cf2`。

archive-index.json逐项记录旧路径、ZIP成员名、大小和SHA。ZIP内_manifest.json也保留相同清单。恢复时按需解压到临时目录，不覆盖当前资料。

普通检查：python tools/checks/check_docs.py；完整归档检查：python tools/checks/check_docs.py --archives。仅克隆Git不含私有历史包，需保留backups副本。
