//
// Created by Sharun E Rajeev on 14-08-2022.
//

#include "ql_oe.h" // Contains all the required functions and constants for running the audio program

/**
 * param '*offset' value is the first wav data position
 * @Note if offset == NULL, then 'offset' should be -1 when call Ql_AudioPlayFileStart()
 */
 // Function that opens the file for reading
int Ql_AudFileOpen(char *file, struct ST_MediaParams *mediaParams, int *offset)
{
    int fd;
    int ret;

    if (file == NULL && mediaParams == NULL)
    {
        QLLOG("%s: args invalid\n", __FUNCTION__);
        goto openfile_failed;
    }

    fd = ql_open_and_convert_file(file);
    if (fd < 0) {
        QLLOG("%s: open file failed\n", __FUNCTION__);
        goto openfile_failed;
    }

    /* check media params */
    switch (mediaParams->format) {
        case AUD_STREAM_FORMAT_AMR:
            __ql_wav_amr_check(fd);
            break;

        case AUD_STREAM_FORMAT_PCM:
            ret = __ql_wav_pcm16Le_check(fd);
            if (ret < 0) {
                QLLOG("parseMediafilePcm16Le failed\n");
                goto openfile_failed;
            }
            if (offset != NULL) {
                *offset = ret;
            }
            break;

        default:
            QLLOG("parse Unknown File\n");
            goto openfile_failed;
    }

    return fd;
    openfile_failed:
    return -1;
}

// Function to print the usage of the commands
static void print_usage(void)
{
    quec_log(
            "\n\r--Usage:\n\r"
            "Play one file: ./<process> play1 <file>\n\r"
            "Play two file: ./<process> play2 <file1> <file2>\n\r"
    );
}

//
// Usage:
//   Play the specified audio file:
//        <process> play1 <audio file>
//   Mixer player:
//        <process> play2 <audio file1> <audio file2>
//

int main(int argc, char** argv) {
    int iRet;
    int devphdl1;//play1
    int devphdl2;//play2
    int filefd1;
    int filefd2;
    struct ST_MediaParams mediaParas;

    unsigned int tmp32;
#ifdef TEST_CODEC_5616
    struct Ql_ALC5616_DRCAGC drcagc;
#endif

    // if the command contains less than 2 arguments return the request message once again.
    if (argc < 2)
    {
        print_usage();
        return 0;
    }

    // Play the specified audio file
    // Command: <process> play1 <audio file>
    if (strcmp(argv[1], "play1") == 0)
    {
        if (argc != 3)
        {
            quec_log("< player needs more parameter\n\r");
            return 0;
        }

        mediaParas.format = AUD_STREAM_FORMAT_PCM; // AUD_STREAM_FORMAT_PCM is defined in the 'ql_oe.h'
        filefd1 = Ql_AudFileOpen(argv[2], &mediaParas, NULL);

        // TODO: Recheck the meaning of the code
        Ql_clt_set_mixer_value("SEC_AUX_PCM_RX Audio Mixer MultiMedia1", 1, "1");
        //  Ql_clt_set_mixer_value("MultiMedia1 Mixer SEC_AUX_PCM_UL_TX", 1, "1");

        // Ql_AudPlayer_Open function writes pcm data to pcm device to play.
        devphdl1 = Ql_AudPlayer_Open(NULL, Ql_cb_Player1);
        // returns -1 if it fails to open the audio player
        if (devphdl1 < 0) {
            quec_log("open aud play%d failed\n\r", devphdl1);
            return 0;
        }

// Checking for codec
// TODO: May not require this code
#ifdef TEST_CODEC_5616 // ALC5616->Ultra-Low Power Audio CODEC
        drcagc.control1_mask = 0xffff;
        drcagc.control1_value = 0x4326;
        drcagc.control2_mask = 0xffff;
        drcagc.control2_value = 0x1fe5;
        drcagc.control3_mask = 0xffff;
        drcagc.control3_value = 0x0580;
        if(Ql_AudCodec_Set_ALC5616_DRCAGC(NULL, &drcagc) < 0) {
            quec_log("Ql_AudCodec_Set_ALC5616_DRCAGC failed\n\r");
        } else {
            quec_log("Ql_AudCodec_Set_ALC5616_DRCAGC success\n\r");
        }
#endif
        play1Flag = 0;
        iRet = Ql_AudPlayer_PlayFrmFile(devphdl1, filefd1, -1); // function plays the pcm data from the specified file.
        if (iRet != 0) {
            quec_log("start audio play%d failed\n\r", devphdl1);
        }

        while (play1Flag != 1) {
            sleep(1);
        }

        Ql_AudPlayer_Stop(devphdl1);
        Ql_AudPlayer_Close(devphdl1); // function that closes player, and free the resource. <ql_audio.h>
    }

}