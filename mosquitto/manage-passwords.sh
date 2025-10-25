#!/bin/bash
# Mosquitto Password Management Script

set -e

COLORS='\033[0m'
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'

# Helper functions
log_info() {
    echo -e "${BLUE}ℹ${COLORS} $1"
}

log_success() {
    echo -e "${GREEN}✓${COLORS} $1"
}

log_error() {
    echo -e "${RED}✗${COLORS} $1"
}

log_warning() {
    echo -e "${YELLOW}⚠${COLORS} $1"
}

# Check if Docker is available
check_docker() {
    if ! command -v docker &> /dev/null; then
        log_error "Docker is not installed or not in PATH"
        exit 1
    fi
}

# Check if Mosquitto container is running
check_container() {
    if ! docker ps | grep -q mosquitto-broker; then
        log_error "Mosquitto container is not running"
        log_info "Start it with: docker-compose up -d"
        exit 1
    fi
}

# Add a new user
add_user() {
    local username=$1
    local password=$2

    if [ -z "$username" ] || [ -z "$password" ]; then
        log_error "Usage: add_user <username> <password>"
        return 1
    fi

    log_info "Adding user: $username"
    docker exec mosquitto-broker mosquitto_passwd -b /mosquitto/config/passwords.txt "$username" "$password"
    log_success "User '$username' added successfully"
}

# Delete a user
delete_user() {
    local username=$1

    if [ -z "$username" ]; then
        log_error "Usage: delete_user <username>"
        return 1
    fi

    log_info "Deleting user: $username"
    docker exec mosquitto-broker mosquitto_passwd -D /mosquitto/config/passwords.txt "$username"
    log_success "User '$username' deleted successfully"
}

# Change user password
change_password() {
    local username=$1
    local new_password=$2

    if [ -z "$username" ] || [ -z "$new_password" ]; then
        log_error "Usage: change_password <username> <new_password>"
        return 1
    fi

    log_info "Changing password for: $username"
    docker exec mosquitto-broker mosquitto_passwd -b /mosquitto/config/passwords.txt "$username" "$new_password"
    log_success "Password for '$username' changed successfully"
}

# List all users
list_users() {
    log_info "Active users in password file:"
    docker exec mosquitto-broker sh -c 'cat /mosquitto/config/passwords.txt | cut -d: -f1'
}

# Reset password file
reset_passwords() {
    log_warning "This will reset all users!"
    read -p "Are you sure? (y/N): " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        log_info "Resetting password file..."
        docker exec mosquitto-broker sh -c 'rm -f /mosquitto/config/passwords.txt'
        docker exec mosquitto-broker mosquitto_passwd -c -b /mosquitto/config/passwords.txt admin changeme
        log_success "Password file reset. Default user: admin / changeme"
    else
        log_info "Operation cancelled"
    fi
}

# Test MQTT connection
test_connection() {
    local username=$1
    local password=$2
    local topic="${3:-\$SYS/broker/clients/total}"

    if [ -z "$username" ] || [ -z "$password" ]; then
        log_error "Usage: test_connection <username> <password> [topic]"
        return 1
    fi

    log_info "Testing MQTT connection..."
    log_info "Username: $username"
    log_info "Topic: $topic"

    if docker exec mosquitto-broker mosquitto_sub -h localhost -p 1883 \
        -u "$username" -P "$password" -t "$topic" -W 1 &>/dev/null; then
        log_success "Connection successful!"
    else
        log_error "Connection failed!"
        return 1
    fi
}

# Generate secure password
generate_password() {
    local length=${1:-16}
    log_info "Generating secure password (length: $length)..."
    local password=$(openssl rand -base64 "$length" | tr -d '\n' | cut -c1-"$length")
    echo "$password"
}

# Interactive menu
show_menu() {
    echo ""
    echo -e "${BLUE}Mosquitto Password Management${COLORS}"
    echo "==============================="
    echo "1. Add new user"
    echo "2. Delete user"
    echo "3. Change password"
    echo "4. List all users"
    echo "5. Reset all passwords"
    echo "6. Test connection"
    echo "7. Generate password"
    echo "8. Exit"
    echo ""
    read -p "Select option (1-8): " choice
}

# Main interactive mode
interactive_mode() {
    check_docker
    check_container

    while true; do
        show_menu
        case $choice in
            1)
                read -p "Username: " username
                read -sp "Password: " password
                echo
                add_user "$username" "$password"
                ;;
            2)
                read -p "Username to delete: " username
                delete_user "$username"
                ;;
            3)
                read -p "Username: " username
                read -sp "New password: " password
                echo
                change_password "$username" "$password"
                ;;
            4)
                list_users
                ;;
            5)
                reset_passwords
                ;;
            6)
                read -p "Username: " username
                read -sp "Password: " password
                echo
                read -p "Topic (default: \$SYS/broker/clients/total): " topic
                topic=${topic:-\$SYS/broker/clients/total}
                test_connection "$username" "$password" "$topic"
                ;;
            7)
                read -p "Password length (default: 16): " length
                length=${length:-16}
                password=$(generate_password "$length")
                echo -e "${GREEN}Generated password:${COLORS} $password"
                read -p "Copy to clipboard? (y/N): " -n 1 -r
                if [[ $REPLY =~ ^[Yy]$ ]]; then
                    echo "$password" | xclip -selection clipboard 2>/dev/null || echo "$password" | pbcopy 2>/dev/null || log_warning "Could not copy to clipboard"
                fi
                ;;
            8)
                log_info "Goodbye!"
                exit 0
                ;;
            *)
                log_error "Invalid option"
                ;;
        esac
    done
}

# Command line mode
case "${1:-menu}" in
    add)
        check_docker
        check_container
        add_user "$2" "$3"
        ;;
    delete)
        check_docker
        check_container
        delete_user "$2"
        ;;
    change)
        check_docker
        check_container
        change_password "$2" "$3"
        ;;
    list)
        check_docker
        check_container
        list_users
        ;;
    reset)
        check_docker
        check_container
        reset_passwords
        ;;
    test)
        check_docker
        check_container
        test_connection "$2" "$3" "$4"
        ;;
    generate)
        generate_password "$2"
        ;;
    *)
        interactive_mode
        ;;
esac
