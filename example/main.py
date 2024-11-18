import licensepy
import time
import signal, sys


def signal_handler(signum, frame):
    print(f"Signal {signum} received, exiting...")
    licensepy.cleanup()
    sys.exit(0)


signal.signal(signal.SIGINT, signal_handler)  # Ctrl+C
signal.signal(signal.SIGTERM, signal_handler)  # kill
signal.signal(signal.SIGSEGV, signal_handler)  # kill


def app_entry():
    for i in range(200):
        print(f"time {i+10}s")
        time.sleep(10)


def main():
    try:
        result = licensepy.verify("127.0.0.1", 8442, 30)
        print(result)
        if result:
            app_entry()
        
    except Exception as e:
        print(f"unexpected error: {e}")
    finally:
        licensepy.cleanup()


if __name__ == "__main__":
    main()
