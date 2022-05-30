/****************************************************************************
*  \file       drvI2C.c
*
*  \details    Driver whom read date from the SenHat
*
*  \author     Aubert, Gosteli
*
* ***************************************************************************/

#include <linux/module.h>      /* Needed by all modules 					*/
#include <linux/kernel.h>      /* Needed for KERN_INFO 						*/
#include <linux/init.h>        /* Needed for the macros 					*/
#include <linux/fs.h>          /* Header for the Linux file system support 	*/
#include <linux/uaccess.h>     /* Required for the copy to user function	*/
#include <linux/moduleparam.h> /* Needed for module parameters 				*/
#include <linux/device.h>      /* Header to support the kernel Driver Model	*/
#include <linux/kthread.h>     /* Header to support thread                  */
#include <linux/delay.h>       /* Header fo msleep                          */
#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/delay.h>

#define DEVICE_NAME "drvI2C" /* The device will appear at /dev/drvI2C using this value	*/
#define CLASS_NAME "hearc"   /* The device class -- this is a character device driver	*/

#define I2C_BUS_AVAILABLE (1)          // I2C Bus available in our Raspberry Pi
#define SLAVE_DEVICE_NAME ("TCS34725") // Device and Driver Name
#define TCS_SLAVE_ADDR (0x29)          // TCS34725 Slave Address

static int majorNumber;                    /* Device number -- determined automatically			*/
static short size_of_msg;                  /* Used to remember the size of the string stored		*/
static int nbrOpens = 0;                   /* Counts the number of times the device is opened		*/
static struct class *devI2CClass = NULL;   /* The device-driver class struct pointer				*/
static struct device *devI2CDevice = NULL; /* The device-driver device struct pointer				*/

static int data[400] = {-1}; // format i,r,g,b

static struct i2c_adapter *tcs_i2c_adapter = NULL; // I2C Adapter Structure
static struct i2c_client *tcs_i2c_client = NULL;   // I2C Cient Structure

// Prototype functions for the character driver
static int dev_open(struct inode *, struct file *);
static int dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);

// TCS Functions
static void TCS_Write(unsigned char *data);
static void TCS_Read(unsigned char *out_data);
static int TCS_init(void);
static void TCS_Convert_IRGB(char data[8], int *out_buff);
static int tcs_probe(struct i2c_client *client, const struct i2c_device_id *id);
static int tcs_remove(struct i2c_client *client);

/*
** KTHREAD PART
*/

static struct task_struct *ts1;
static int threadRun = 0;

static int kthread_func2(void *arg)
{
    /* Every kthread has a struct task_struct associated with it which is it's identifier.
     * Whenever a thread is schedule for execution, the kernel sets "current" pointer to
     * it's struct task_struct.
     * current->comm is the name of the command that caused creation of this thread
     * current->pid is the process of currently executing thread
     */

    printk(KERN_INFO "I am thread: %s[PID = %d]\n", current->comm, current->pid);
    threadRun = 1;

    int i = 0;
    int max = 400;

    while (threadRun > 0)
    {
        char dt[8];
        TCS_Read(dt);
        int irgbdata[4] = {-1};
        TCS_Convert_IRGB(dt, irgbdata);

        int j = 0;
        while (j < 4)
        {
            data[i++] = irgbdata[j++];
        }

        if (i >= max)
        {
            i = 0;
        }

        msleep(500);
    }

    return 0;
}

/*
** DEV PART
*/

/** @brief Devices are represented as file structure in the kernel.
 *  The file_operations structure from /linux/fs.h lists the callback functions that you wish to
 *  associated with your file operations using a C99 syntax structure.
 *  char devices usually implement open, read, write and release calls
 */
static struct file_operations fops =
    {
        .open = dev_open,
        .read = dev_read,
        // .write = dev_write,
        .release = dev_release,
};

/** @brief The device open function that is called each time the device is opened
 *  This will only increment the nbrOpens counter in this case.
 *  @param inodep A pointer to an inode object (defined in linux/fs.h)
 *  @param filep A pointer to a file object (defined in linux/fs.h)
 */
static int dev_open(struct inode *inodep, struct file *filep)
{
    nbrOpens++;
    pr_info("devTest: Device has been opened %d time(s)\n", nbrOpens);
    return 0;
}

/** @brief This function is called whenever device is being read from user space i.e. data is
 *  being sent from the device to the user. In this case is uses the copy_to_user() function to
 *  send the buffer string to the user and captures any errors.
 *  @param filep A pointer to a file object (defined in linux/fs.h)
 *  @param buffer The pointer to the buffer to which this function writes the data
 *  @param len The length of the b
 *  @param offset The offset if required
 */
static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset)
{
    int error_count = 0;
    // copy_to_user has the format ( * to, *from, size) and returns 0 on success
    

    error_count = copy_to_user(buffer, (char*)data, sizeof(int)*400);
    
    if (error_count == 0)
    { // if true then have success
        pr_info("devI2C: Sent %d characters to the user\n", sizeof(buffer)/sizeof(char));
        return 0; // clear the position to the start and return 0
    }
    else
    {
        pr_info("devI2C: Failed to send %d characters to the user\n", error_count);
        return -EFAULT; // Failed -- return a bad address message (i.e. -14)
    }
}

/** @brief This function is called whenever the device is being written to from user space i.e.
 *  data is sent to the device from the user. The data is copied to the message[] array in this
 *  using the sprintf() function along with the length of the string.
 *  @param filep A pointer to a file object
 *  @param buffer The buffer to that contains the string to write to the device
 *  @param len The length of the array of data that is being passed in the const char buffer
 *  @param offset The offset if required
 */
// static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset)
// {
//     sprintf(message, "%s(%zu letters)", buffer, len); // appending received string with its length
//     size_of_msg = strlen(message);                    // store the length of the stored message
//     pr_info("devTest: Received %zu characters from the user\n", len);
//     return len;
// }

/** @brief The device release function that is called whenever the device is closed/released by
 *  the userspace program
 *  @param inodep A pointer to an inode object (defined in linux/fs.h)
 *  @param filep A pointer to a file object (defined in linux/fs.h)
 */
static int dev_release(struct inode *inodep, struct file *filep)
{
    pr_info("devTest: Device closed\n");
    return 0;
}

/*
** I2C PART
*/

/*
** This function writes the data into the I2C client
**
**  Arguments:
**      buff -> buffer to be sent
**      len  -> Length of the data
**
*/
static int I2C_Write(unsigned char *buf, unsigned int len)
{
    /*
    ** Sending Start condition, Slave address with R/W bit,
    ** ACK/NACK and Stop condtions will be handled internally.
    */
    int ret = i2c_master_send(tcs_i2c_client, buf, len);

    return ret;
}

/*
** This function reads one byte of the data from the I2C client
**
**  Arguments:
**      out_buff -> buffer wherer the data to be copied
**      len      -> Length of the data to be read
**
*/
static int I2C_Read(unsigned char *out_buf, unsigned int len)
{
    /*
    ** Sending Start condition, Slave address with R/W bit,
    ** ACK/NACK and Stop condtions will be handled internally.
    */
    int ret = i2c_master_recv(tcs_i2c_client, out_buf, len);

    return ret;
}

/*
**  TCS PART
*/

/*
** This function sends the command/data to the TCS.
**
**  Arguments:
**      data   -> data to be written
**
*/
static void TCS_Write(unsigned char *data)
{
    int ret;

    ret = I2C_Write(data, 2);
}

/*
** This function reads the data to the TCS.
**
**  Arguments:
**      data   -> data to be read
**
*/
static void TCS_Read(unsigned char *out_data)
{
    int ret;
    // Read 8 bytes of data from register(0x94)
    // cData lsb, cData msb, red lsb, red msb, green lsb, green msb, blue lsb, blue msb
    char reg[1] = {0x94};

    ret = I2C_Write(reg, 1);
    ret = I2C_Read(out_data, 8);
}

/*
** This function sends the commands that need to used to Initialize the TCS.
**
**  Arguments:
**      none
**
*/
static int TCS_init(void)
{
    char config[2] = {0};
    char data[8] = {0};

    msleep(10); // delay

    //
    // Commands to initialize the TCS
    //

    // Select enable register(0x80)
    // Power ON, RGBC enable, wait time disable(0x03)
    config[0] = 0x80;
    config[1] = 0x03;
    TCS_Write(config);
    // Select ALS time register(0x81)
    // Atime = 700 ms(0x00)
    config[0] = 0x81;
    config[1] = 0x00;
    TCS_Write(config);
    // Select Wait Time register(0x83)
    // WTIME : 2.4ms(0xFF)
    config[0] = 0x83;
    config[1] = 0xFF;
    TCS_Write(config);
    // Select control register(0x8F)
    // AGAIN = 1x(0x00)
    config[0] = 0x8F;
    config[1] = 0x00;
    TCS_Write(config);
    msleep(10);

    TCS_Read(data);
    int irgbdata[4] = {0};
    TCS_Convert_IRGB(data, irgbdata);

    // Output data to screen
    pr_info("Red color luminance : %d lux \n", irgbdata[1]);
    pr_info("Green color luminance : %d lux \n", irgbdata[2]);
    pr_info("Blue color luminance : %d lux \n", irgbdata[3]);
    pr_info("IR  luminance : %d lux \n", irgbdata[0]);
    return 0;
}

/**
*** @brief This Function convert TCS data to match standard IRGB value
***
**/
static void TCS_Convert_IRGB(char data[8], int *out_buff)
{
    out_buff[0] = (data[1] * 256 + data[0]);
    out_buff[1] = (data[3] * 256 + data[2]);
    out_buff[2] = (data[5] * 256 + data[4]);
    out_buff[3] = (data[7] * 256 + data[6]);
}

/*
** This function getting called when the slave has been found
** Note : This will be called only once when we load the driver.
*/
static int tcs_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
    pr_info("TCS Probed!!!\n");
    return 0;
}

/*
** This function getting called when the slave has been removed
** Note : This will be called only once when we unload the driver.
*/
static int tcs_remove(struct i2c_client *client)
{
    pr_info("TCS Removed!!!\n");
    return 0;
}

/*
** Structure that has slave device id
*/
static const struct i2c_device_id tcs_id[] = {
    {SLAVE_DEVICE_NAME, 0},
    {}};
MODULE_DEVICE_TABLE(i2c, tcs_id);

/*
** I2C driver Structure that has to be added to linux
*/
static struct i2c_driver tcs_driver = {
    .driver = {
        .name = SLAVE_DEVICE_NAME,
        .owner = THIS_MODULE,
    },
    .probe = tcs_probe,
    .remove = tcs_remove,
    .id_table = tcs_id,
};

/*
** I2C Board Info strucutre
*/
static struct i2c_board_info tcs_i2c_board_info = {
    I2C_BOARD_INFO(SLAVE_DEVICE_NAME, TCS_SLAVE_ADDR)};

/*
** Module Init function
*/
static int __init tcs_driver_init(void)
{
    int ret = -1;

    majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
    if (majorNumber < 0)
    {
        pr_alert("devI2C: failed to register a major number\n");
        return majorNumber;
    }
    pr_info("devI2C: registered (major number %d)\n", majorNumber);

    // Register the device class
    devI2CClass = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(devI2CClass))
    { // Check for error and clean up if there is
        unregister_chrdev(majorNumber, DEVICE_NAME);
        pr_alert("Failed to register device class\n");
        return PTR_ERR(devI2CClass); // Return an error on a pointer
    }
    pr_info("devI2C: device class registered\n");

    // Register the device driver
    devI2CDevice = device_create(devI2CClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
    if (IS_ERR(devI2CDevice))
    { // Clean up if there is an error
        class_destroy(devI2CClass);
        unregister_chrdev(majorNumber, DEVICE_NAME);
        pr_alert("devI2C: Failed to create the device\n");
        return PTR_ERR(devI2CDevice);
    }
    pr_info("devI2C: device class created\n");

    tcs_i2c_adapter = i2c_get_adapter(I2C_BUS_AVAILABLE);

    if (tcs_i2c_adapter != NULL)
    {
        tcs_i2c_client = i2c_new_client_device(tcs_i2c_adapter, &tcs_i2c_board_info);
        if (tcs_i2c_client != NULL)
        {
            i2c_add_driver(&tcs_driver);
            ret = 0;
        }

        i2c_put_adapter(tcs_i2c_adapter);
    }
    TCS_init();
    pr_info("Driver Added !!\n");

    msleep(100); // delay

    int err;
    ts1 = kthread_run(kthread_func2, NULL, "thread-1");
    if (IS_ERR(ts1))
    {
        printk(KERN_INFO "ERROR: Cannot create thread ts1\n");
        err = PTR_ERR(ts1);
        ts1 = NULL;
        return err;
    }

    return ret;
}

/*
** Module Exit function
*/
static void __exit tcs_driver_exit(void)
{

    threadRun = -1;
    msleep(2500);

    device_destroy(devI2CClass, MKDEV(majorNumber, 0)); // remove the device
    class_unregister(devI2CClass);                      // unregister the device class
    class_destroy(devI2CClass);                         // remove the device class
    unregister_chrdev(majorNumber, DEVICE_NAME);        // unregister the major number

    i2c_unregister_device(tcs_i2c_client);
    i2c_del_driver(&tcs_driver);
    pr_info("Driver Removed!!!\n");
}

module_init(tcs_driver_init);
module_exit(tcs_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Gosteli <lucas.gosteli@he-arc.ch>, Aubert <nicolas.aubert@he-arc.ch>");
MODULE_DESCRIPTION("Driver to read sensors on the SensHat");
MODULE_VERSION("0.1");
