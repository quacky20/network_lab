# NS-3 Lab 8 - Monitoring Packet Drops and Queueing Effects

This README provides step-by-step instructions on how to set up and run the lab assignments in NS-3 for **Understanding Bottleneck and Queueing Effects**.

## How to Run the Lab Assignments

### 1. **Open a Terminal**
   - Open your terminal application (e.g., Terminal on Linux/macOS or Command Prompt/PowerShell on Windows).

### 2. **Navigate to the NS-3 Directory**
   - Use the `cd` command to navigate to the directory where NS-3 is installed.
   ```bash
   cd /path/to/ns-3
   ```
### 3. **Build NS-3**
   - Run the following command to build NS-3:
   ```bash
   ./ns3 build
   ```
	This step will compile the necessary files and dependencies for the simulations

### 4. Run Each of the Assignment Files**

	- For each assignment (1 to 4), you need to run the corresponding script. The format for running a simulation is:
	```bash
	./ns3 run lab8_<assignment_no>
	```
	Replace <assignment_no> with the number corresponding to the assignment. For example:

	To run Assignment 1:
	```bash
	./ns3 run lab8_1
	```
	To run Assignment 2:
	```bash
	./ns3 run lab8_2
	```
	Repeat the same process for the other assignments.
