## Data Forwarding Table

| Pipeline register containing source instruction | Opcode of source instruction | Pipeline register containing destination instruction | Opcode of destination instruction | Destination of the forwarded result | Comparison (if equal then forward) |
|-------------------------------------------------|------------------------------|------------------------------------------------------|-----------------------------------|------------------------------------|------------------------------------|
| EX/MEM                                          | Register-register ALU         | ID/EX                                                | Register-register ALU, ALU immediate, load, store, branch | Top ALU input                      | EX/MEM.IR[16..20] == ID/EX.IR[6..10] |
| EX/MEM                                          | Register-register ALU         | ID/EX                                                | Register-register ALU, ALU immediate, load, store, branch | Bottom ALU input                   | EX/MEM.IR[11..15] == ID/EX.IR[11..15] |
| MEM/WB                                          | Register-register ALU         | ID/EX                                                | Register-register ALU, ALU immediate, load, store, branch | Top ALU input                      | MEM/WB.IR[16..20] == ID/EX.IR[6..10] |
| MEM/WB                                          | Register-register ALU         | ID/EX                                                | Register-register ALU, ALU immediate, load, store, branch | Bottom ALU input                   | MEM/WB.IR[11..15] == ID/EX.IR[11..15] |
| EX/MEM                                          | ALU immediate                 | ID/EX                                                | Register-register ALU, ALU immediate, load, store, branch | Top ALU input                      | EX/MEM.IR[11..15] == ID/EX.IR[6..10] |
| EX/MEM                                          | ALU immediate                 | ID/EX                                                | Register-register ALU, ALU immediate, load, store, branch | Bottom ALU input                   | EX/MEM.IR[11..15] == ID/EX.IR[11..15] |
| MEM/WB                                          | ALU immediate                 | ID/EX                                                | Register-register ALU, ALU immediate, load, store, branch | Top ALU input                      | MEM/WB.IR[16..20] == ID/EX.IR[6..10] |
| MEM/WB                                          | ALU immediate                 | ID/EX                                                | Register-register ALU, ALU immediate, load, store, branch | Bottom ALU input                   | MEM/WB.IR[11..15] == ID/EX.IR[11..15] |
| MEM/WB                                          | Load                          | ID/EX                                                | Register-register ALU, ALU immediate, load, store, branch | Top ALU input                      | MEM/WB.IR[16..20] == ID/EX.IR[6..10] |
| MEM/WB                                          | Load                          | ID/EX                                                | Register-register ALU, ALU immediate, load, store, branch | Bottom ALU input                   | MEM/WB.IR[11..15] == ID/EX.IR[11..15] |


## Load Interlock Detection Table

| Opcode field of ID/EX (ID/EX.IR[0..5]) | Opcode field of IF/ID (IF/ID.IR[0..5])  | Matching operand fields                       |
|----------------------------------------|----------------------------------------|----------------------------------------------|
| Load                                   | Register-register ALU                  | ID/EX.IR[11..15] == IF/ID.IR[6..10]          |
| Load                                   | Register-register ALU                  | ID/EX.IR[11..15] == IF/ID.IR[11..15]         |
| Load                                   | Load, store, ALU immediate, or branch  | ID/EX.IR[11..15] == IF/ID.IR[6..10]          |

