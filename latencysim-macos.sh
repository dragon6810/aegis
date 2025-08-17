#!/usr/bin/env bash
set -euo pipefail

ANCHOR_NAME="delay19771"
PIPE_ID=19771
MAIN_RULES="/tmp/pf_${ANCHOR_NAME}_main.conf"

usage() {
  echo "Usage:"
  echo "  sudo $0 start <ms>   # add <ms> latency to UDP port 19771 on localhost"
  echo "  sudo $0 stop         # remove latency and restore default PF rules"
  echo "  sudo $0 status       # show pf and dnctl status"
  exit 1
}

require_root() {
  if [[ $EUID -ne 0 ]]; then
    echo "Please run as root (use sudo)."
    exit 1
  fi
}

start() {
  local ms="${1:-}"
  [[ -z "$ms" ]] && usage
  [[ "$ms" =~ ^[0-9]+$ ]] || { echo "Latency must be an integer (ms)."; exit 1; }

  # Configure dummynet pipe
  dnctl -q pipe delete ${PIPE_ID} 2>/dev/null || true
  dnctl pipe ${PIPE_ID} config delay ${ms}ms

  # Minimal main PF ruleset w/ anchor (no 'set skip on lo0')
  cat > "${MAIN_RULES}" <<EOF
dummynet-anchor "${ANCHOR_NAME}"
anchor "${ANCHOR_NAME}"

pass in all
pass out all
EOF

  # Load main rules and enable PF
  pfctl -f "${MAIN_RULES}"
  pfctl -E >/dev/null || true

  # Load anchor rules that send lo0:19771 UDP traffic into the pipe (both directions)
  pfctl -a "${ANCHOR_NAME}" -f - <<EOF
dummynet in  quick on lo0 proto udp from 127.0.0.1 to 127.0.0.1 port = 19771 pipe ${PIPE_ID}
dummynet out quick on lo0 proto udp from 127.0.0.1 to 127.0.0.1 port = 19771 pipe ${PIPE_ID}
EOF

  echo "UDP latency enabled: ${ms}ms on 127.0.0.1:19771"
}

stop() {
  # Clear anchor rules and dummynet pipe
  pfctl -a "${ANCHOR_NAME}" -F all 2>/dev/null || true
  dnctl -q pipe delete ${PIPE_ID} 2>/dev/null || true

  # Restore the system default PF config (includes 'set skip on lo0')
  if [[ -f /etc/pf.conf ]]; then
    pfctl -f /etc/pf.conf
  fi

  echo "Cleared delay on 127.0.0.1:19771 and restored PF rules."
  echo "You can also disable PF entirely with: sudo pfctl -d"
}

status() {
  echo "----- PF enabled? -----"
  pfctl -s info | grep "Status:"
  echo "----- PF rules (top-level) -----"
  pfctl -sr
  echo "----- Anchor ${ANCHOR_NAME} -----"
  pfctl -a "${ANCHOR_NAME}" -sr || true
  echo "----- dummynet pipes -----"
  dnctl list 2>/dev/null || true
}

main() {
  [[ $# -lt 1 ]] && usage
  require_root
  case "$1" in
    start) shift; start "$@";;
    stop)  stop;;
    status) status;;
    *) usage;;
  esac
}

main "$@"
