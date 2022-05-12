#include <iostream>
#include <string>

#include <vlcpp/vlc.hpp>
#include <vlcpp/MediaPlayer.hpp>

const std::string cam_ip = "192.168.178.30";    /**< Camera's IP address */
const std::string cam_port = "554";             /**< Camera's RTSP server port */

/**
 * @brief print debug string if DEBUG flag is enabled
 * 
 * @param s string to be printed
 */
void debug_str(std::string s) {
    #ifdef DEBUG
    std::cout << s << std::endl;
    #endif
}

int main() {
    debug_str("Entering main");

    std::string url = "rtsp://" + cam_ip + ":" + cam_port;
    const char* const options[] = {
        "--sout-rtsp-user", "admin",
        "--sout-rtsp-pwd", "password"};
    auto instance = VLC::Instance(4, options);
    auto md = VLC::Media(instance, url, VLC::Media::FromLocation);
    auto player = VLC::MediaPlayer(md);
    player.play();

    debug_str("Exiting from main");
    return 0;
}