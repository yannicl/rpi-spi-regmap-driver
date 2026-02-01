#include <linux/module.h>
#include <linux/spi/spi.h>
#include <linux/regmap.h>
#include <linux/of.h>

static int pico_spi_write(void *context, const void *data, size_t count)
{
    struct spi_device *spi = context;

    return spi_write(spi, data, count);
}

static int pico_spi_read(void *context, const void *reg,
                         size_t reg_size, void *val, size_t val_size)
{
    struct spi_device *spi = context;
    struct spi_transfer xfers[2] = { };
    struct spi_message msg;

    spi_message_init(&msg);

    xfers[0].tx_buf = reg;
    xfers[0].len    = reg_size;
    spi_message_add_tail(&xfers[0], &msg);

    xfers[1].rx_buf = val;
    xfers[1].len    = val_size;
    spi_message_add_tail(&xfers[1], &msg);

    return spi_sync(spi, &msg);
}

struct pico_data {
    struct regmap *regmap;
};

static ssize_t reg_show(struct device *dev,
                        struct device_attribute *attr,
                        char *buf)
{
    struct spi_device *spi = to_spi_device(dev);
    struct pico_data *data = spi_get_drvdata(spi);
    u32 val;
    int ret;

    ret = regmap_read(data->regmap, 0x00, &val);
    if (ret)
        return ret;

    return sysfs_emit(buf, "0x%08x\n", val);
}

static ssize_t reg_store(struct device *dev,
                         struct device_attribute *attr,
                         const char *buf, size_t count)
{
    struct spi_device *spi = to_spi_device(dev);
    struct pico_data *data = spi_get_drvdata(spi);
    u32 val;
    int ret;

    ret = kstrtou32(buf, 0, &val);
    if (ret)
        return ret;

    ret = regmap_write(data->regmap, 0x00, val);
    if (ret)
        return ret;

    return count;
}

static DEVICE_ATTR_RW(reg);

static const struct regmap_bus pico_regmap_bus = {
    .write = pico_spi_write,
    .read  = pico_spi_read,
};



static const struct regmap_config pico_regmap_cfg = {
    .reg_bits       = 8,
    .val_bits       = 32,
    .read_flag_mask = 0x80,
    .max_register   = 0xFF,
};

static int pico_spi_remove(struct spi_device *spi)
{
    device_remove_file(&spi->dev, &dev_attr_reg);
    return 0;
}

static int pico_spi_probe(struct spi_device *spi)
{
    struct pico_data *data;
    int ret;

    spi->mode = SPI_MODE_0;
    spi->bits_per_word = 8;

    ret = spi_setup(spi);
    if (ret)
        return ret;

    data = devm_kzalloc(&spi->dev, sizeof(*data), GFP_KERNEL);
    if (!data)
        return -ENOMEM;

    data->regmap = devm_regmap_init(&spi->dev, &pico_regmap_bus,
                 spi, &pico_regmap_cfg);
    if (IS_ERR(data->regmap))
        return PTR_ERR(data->regmap);

    spi_set_drvdata(spi, data);

    ret = device_create_file(&spi->dev, &dev_attr_reg);
    if (ret)
        return ret;
    
    dev_info(&spi->dev, "Pico SPI regmap driver loaded\n");
    return 0;
}

static const struct of_device_id pico_of_match[] = {
    { .compatible = "custom,pico-spi" },
    { }
};
MODULE_DEVICE_TABLE(of, pico_of_match);

static struct spi_driver pico_spi_driver = {
    .driver = {
        .name           = "pico-spi",
        .of_match_table = pico_of_match,
    },
    .probe = pico_spi_probe,
    .remove = pico_spi_remove,
};

module_spi_driver(pico_spi_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Yannic Luyckx");
MODULE_DESCRIPTION("SPI regmap driver for Raspberry Pi Pico");