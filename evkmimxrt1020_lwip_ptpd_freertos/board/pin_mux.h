/*
 * Copyright 2018, 2021, 2023 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/***********************************************************************************************************************
 * This file was generated by the MCUXpresso Config Tools. Any manual edits made to this file
 * will be overwritten if the respective MCUXpresso Config Tools is used to update this file.
 **********************************************************************************************************************/

#ifndef _PIN_MUX_H_
#define _PIN_MUX_H_

/***********************************************************************************************************************
 * Definitions
 **********************************************************************************************************************/

/*! @brief Direction type  */
typedef enum _pin_mux_direction
{
  kPIN_MUX_DirectionInput = 0U,         /* Input direction */
  kPIN_MUX_DirectionOutput = 1U,        /* Output direction */
  kPIN_MUX_DirectionInputOrOutput = 2U  /* Input or output direction */
} pin_mux_direction_t;

/*!
 * @addtogroup pin_mux
 * @{
 */

/***********************************************************************************************************************
 * API
 **********************************************************************************************************************/

#if defined(__cplusplus)
extern "C" {
#endif

/*!
 * @brief Calls initialization functions.
 *
 */
void BOARD_InitBootPins(void);

/* GPIO_AD_B0_04 (number 107), PHY_RESET */
/* Routed pin properties */
#define BOARD_INITPINS_PHY_RESET_PERIPHERAL                                GPIO1   /*!< Peripheral name */
#define BOARD_INITPINS_PHY_RESET_SIGNAL                                  gpio_io   /*!< Signal name */
#define BOARD_INITPINS_PHY_RESET_CHANNEL                                      4U   /*!< Signal channel */

/* Symbols to be used with GPIO driver */
#define BOARD_INITPINS_PHY_RESET_GPIO                                      GPIO1   /*!< GPIO peripheral base pointer */
#define BOARD_INITPINS_PHY_RESET_GPIO_PIN                                     4U   /*!< GPIO pin number */
#define BOARD_INITPINS_PHY_RESET_GPIO_PIN_MASK                        (1U << 4U)   /*!< GPIO pin mask */
#define BOARD_INITPINS_PHY_RESET_PORT                                      GPIO1   /*!< PORT peripheral base pointer */
#define BOARD_INITPINS_PHY_RESET_PIN                                          4U   /*!< PORT pin number */
#define BOARD_INITPINS_PHY_RESET_PIN_MASK                             (1U << 4U)   /*!< PORT pin mask */

/* GPIO_AD_B1_06 (number 84), PHY_INTR */
/* Routed pin properties */
#define BOARD_INITPINS_PHY_INTR_PERIPHERAL                                 GPIO1   /*!< Peripheral name */
#define BOARD_INITPINS_PHY_INTR_SIGNAL                                   gpio_io   /*!< Signal name */
#define BOARD_INITPINS_PHY_INTR_CHANNEL                                      22U   /*!< Signal channel */

/* Symbols to be used with GPIO driver */
#define BOARD_INITPINS_PHY_INTR_GPIO                                       GPIO1   /*!< GPIO peripheral base pointer */
#define BOARD_INITPINS_PHY_INTR_GPIO_PIN                                     22U   /*!< GPIO pin number */
#define BOARD_INITPINS_PHY_INTR_GPIO_PIN_MASK                        (1U << 22U)   /*!< GPIO pin mask */
#define BOARD_INITPINS_PHY_INTR_PORT                                       GPIO1   /*!< PORT peripheral base pointer */
#define BOARD_INITPINS_PHY_INTR_PIN                                          22U   /*!< PORT pin number */
#define BOARD_INITPINS_PHY_INTR_PIN_MASK                             (1U << 22U)   /*!< PORT pin mask */


/*!
 * @brief Configures pin routing and optionally pin electrical features.
 *
 */
void BOARD_InitPins(void);

#if defined(__cplusplus)
}
#endif

/*!
 * @}
 */
#endif /* _PIN_MUX_H_ */

/***********************************************************************************************************************
 * EOF
 **********************************************************************************************************************/
