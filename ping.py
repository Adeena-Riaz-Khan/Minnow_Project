import re
import matplotlib.pyplot as plt

def read_ping_file(filename):
    pings = []
    file = open(filename, 'r')
    for line in file:
        timestamp_part = re.search(r'\[(\d+\.\d+)\]', line)
        seq_part = re.search(r'icmp_seq=(\d+)', line)
        time_part = re.search(r'time=([\d.]+)\s*ms', line)
        
        if timestamp_part and seq_part and time_part:
            time_val = float(timestamp_part.group(1))
            seq_val = int(seq_part.group(1))
            rtt_val = float(time_part.group(1))
            if rtt_val > 0 and rtt_val < 10000:
                pings.append({'time': time_val, 'seq': seq_val, 'rtt': rtt_val})
    file.close()
    return pings

print("Reading ping data...")
all_pings = read_ping_file('data.txt')
print(f"Found {len(all_pings)} replies")

seq_numbers = []
rtt_values = []
time_values = []

for p in all_pings:
    seq_numbers.append(p['seq'])
    rtt_values.append(p['rtt'])
    time_values.append(p['time'])

first_seq = min(seq_numbers)
last_seq = max(seq_numbers)
total_sent = last_seq - first_seq + 1

print(f"First sequence: {first_seq}")
print(f"Last sequence: {last_seq}")
print(f"Total sent: {total_sent}")
print(f"Total received: {len(all_pings)}")

delivery = len(all_pings) / total_sent
loss = 1.0 - delivery

print("\n--- Question 1 ---")
print(f"Delivery rate: {delivery:.4f}")
print(f"Packet loss: {loss:.4f}")

received = set(seq_numbers)
all_seqs = set(range(first_seq, last_seq + 1))
lost = all_seqs - received

print("\n--- Question 2 ---")
longest_success = 0
current_success = 0
for s in range(first_seq, last_seq + 1):
    if s in received:
        current_success = current_success + 1
        if current_success > longest_success:
            longest_success = current_success
    else:
        current_success = 0
print(f"Longest success streak: {longest_success}")

print("\n--- Question 3 ---")
longest_loss = 0
current_loss = 0
for s in range(first_seq, last_seq + 1):
    if s not in received:
        current_loss = current_loss + 1
        if current_loss > longest_loss:
            longest_loss = current_loss
    else:
        current_loss = 0
print(f"Longest loss burst: {longest_loss}")

print("\n--- Question 4 ---")
success_after_success_count = 0
total_after_success = 0
success_after_loss_count = 0
total_after_loss = 0

for s in range(first_seq, last_seq):
    current_ok = s in received
    next_ok = (s + 1) in received
    
    if current_ok:
        total_after_success = total_after_success + 1
        if next_ok:
            success_after_success_count = success_after_success_count + 1
    else:
        total_after_loss = total_after_loss + 1
        if next_ok:
            success_after_loss_count = success_after_loss_count + 1

prob_after_success = success_after_success_count / total_after_success
prob_after_loss = success_after_loss_count / total_after_loss

print(f"P(success after success) = {prob_after_success:.4f}")
print(f"P(success after loss) = {prob_after_loss:.4f}")
print(f"Overall delivery = {delivery:.4f}")

difference = prob_after_success - prob_after_loss
if difference < 0.05 and difference > -0.05:
    print("Losses are INDEPENDENT")
else:
    print("Losses are CORRELATED")

print("\n--- Question 5 ---")
min_rtt = min(rtt_values)
print(f"Minimum RTT: {min_rtt:.2f} ms")

print("\n--- Question 6 ---")
max_rtt = max(rtt_values)
print(f"Maximum RTT: {max_rtt:.2f} ms")

print("\n--- Question 7 ---")
plt.figure(figsize=(14, 6))
plt.plot(time_values, rtt_values, linewidth=0.5, alpha=0.7)
plt.xlabel('Time (seconds from start)', fontsize=12)
plt.ylabel('RTT (ms)', fontsize=12)
plt.title('RTT over Time', fontsize=14)
plt.grid(True, alpha=0.3)
plt.ylim(0, 1000)
plt.savefig('rtt_vs_time.png', dpi=200)
print("Saved rtt_vs_time.png")
plt.close()

plt.figure(figsize=(14, 6))
plt.plot(time_values, rtt_values, linewidth=0.5, alpha=0.7, color='red')
plt.xlabel('Time (seconds from start)', fontsize=12)
plt.ylabel('RTT (ms)', fontsize=12)
plt.title('RTT over Time (Full Range with Outliers)', fontsize=14)
plt.grid(True, alpha=0.3)
plt.savefig('rtt_vs_time_full.png', dpi=200)
print("Saved rtt_vs_time_full.png (includes all outliers)")
plt.close()

print("\n--- Question 8 ---")
total = 0
for r in rtt_values:
    total = total + r
mean_rtt = total / len(rtt_values)

sorted_rtt = sorted(rtt_values)
middle = len(sorted_rtt) // 2
median_rtt = sorted_rtt[middle]

print(f"Mean RTT: {mean_rtt:.2f} ms")
print(f"Median RTT: {median_rtt:.2f} ms")

plt.figure(figsize=(12, 5))

plt.subplot(1, 2, 1)
plt.hist(rtt_values, bins=50)
plt.xlabel('RTT (ms)')
plt.ylabel('Count')
plt.title('RTT Histogram')
plt.grid(True)

plt.subplot(1, 2, 2)
y_vals = []
for i in range(len(sorted_rtt)):
    y_vals.append((i + 1) / len(sorted_rtt))
plt.plot(sorted_rtt, y_vals)
plt.xlabel('RTT (ms)')
plt.ylabel('Probability')
plt.title('RTT CDF')
plt.grid(True)

plt.tight_layout()
plt.savefig('rtt_distribution.png')
print("Saved rtt_distribution.png")
plt.close()

print("\n--- Question 9 ---")
rtt_current = []
rtt_next = []
for i in range(len(rtt_values) - 1):
    rtt_current.append(rtt_values[i])
    rtt_next.append(rtt_values[i + 1])

plt.figure(figsize=(8, 8))
plt.scatter(rtt_current, rtt_next, alpha=0.3, s=1)
plt.xlabel('RTT of ping N (ms)')
plt.ylabel('RTT of ping N+1 (ms)')
plt.title('RTT Correlation')
plt.grid(True)
plt.savefig('rtt_correlation.png')
print("Saved rtt_correlation.png")
plt.close()

sum_xy = 0
sum_x = 0
sum_y = 0
sum_x2 = 0
sum_y2 = 0
n = len(rtt_current)

for i in range(n):
    x = rtt_current[i]
    y = rtt_next[i]
    sum_xy = sum_xy + (x * y)
    sum_x = sum_x + x
    sum_y = sum_y + y
    sum_x2 = sum_x2 + (x * x)
    sum_y2 = sum_y2 + (y * y)

numerator = (n * sum_xy) - (sum_x * sum_y)
denominator = ((n * sum_x2 - sum_x * sum_x) * (n * sum_y2 - sum_y * sum_y)) ** 0.5
correlation = numerator / denominator

print(f"Correlation: {correlation:.4f}")

result_file = open('results.txt', 'w')
result_file.write("PING ANALYSIS RESULTS\n")
result_file.write("=" * 50 + "\n\n")
result_file.write(f"Host: www.apple.com\n")
result_file.write(f"Total sent: {total_sent}\n")
result_file.write(f"Total received: {len(all_pings)}\n\n")
result_file.write(f"Q1: Delivery rate = {delivery:.4f}\n")
result_file.write(f"Q1: Loss rate = {loss:.4f}\n\n")
result_file.write(f"Q2: Longest success = {longest_success}\n\n")
result_file.write(f"Q3: Longest loss = {longest_loss}\n\n")
result_file.write(f"Q4: P(success|success) = {prob_after_success:.4f}\n")
result_file.write(f"Q4: P(success|loss) = {prob_after_loss:.4f}\n\n")
result_file.write(f"Q5: Min RTT = {min_rtt:.2f} ms\n\n")
result_file.write(f"Q6: Max RTT = {max_rtt:.2f} ms\n\n")
result_file.write(f"Q7: See rtt_vs_time.png\n\n")
result_file.write(f"Q8: Mean = {mean_rtt:.2f} ms\n")
result_file.write(f"Q8: Median = {median_rtt:.2f} ms\n")
result_file.write(f"Q8: See rtt_distribution.png\n\n")
result_file.write(f"Q9: Correlation = {correlation:.4f}\n")
result_file.write(f"Q9: See rtt_correlation.png\n")
result_file.close()

print("\nSaved results.txt")
print("\nDone!")
