#include <kdef.h>
#include <io.h>
#include <kprint.h>
#include <irq_handle.h>
#include <idt.h>

#define KERN_CODE_SEGMENT 0x08
#define TYPE_32_INT_GATE 0x8E

extern void load_idt(unsigned long* idt_ptr);

static idt_entry idt[256];
static idt_reg_desc_t idt_desc;

static void set_idt_entry(unsigned int int_num, uint32_t address,
			  uint16_t selector, uint8_t type)
{
	struct idt_entry entry;

	entry.offset_lower = address & 0xFFFF;
	entry.selector = selector;
	entry.zero = 0;
	entry.type = type;
	entry.offset_higher = (address & 0xFFFF0000) >> 16;
	
	idt[int_num] = entry;
}

static void set_kern_interrupt(unsigned int int_num, uint32_t address)
{
	set_idt_entry(int_num, address, KERN_CODE_SEGMENT, 
		      TYPE_32_INT_GATE);
}

idt_reg_desc_t * get_idt_desc()
{
	return &idt_desc;
}

void set_idt(void) {
	unsigned long idt_address;
	unsigned long idt_ptr[2];

	set_kern_interrupt(0, (unsigned long) interrupt_handler_0);
	set_kern_interrupt(1, (unsigned long) interrupt_handler_1);
	set_kern_interrupt(2, (unsigned long) interrupt_handler_2);
	set_kern_interrupt(3, (unsigned long) interrupt_handler_3);
	set_kern_interrupt(4, (unsigned long) interrupt_handler_4);
	set_kern_interrupt(5, (unsigned long) interrupt_handler_5);
	set_kern_interrupt(6, (unsigned long) interrupt_handler_6);
	set_kern_interrupt(7, (unsigned long) interrupt_handler_7);
	set_kern_interrupt(8, (unsigned long) interrupt_handler_8);
	set_kern_interrupt(10, (unsigned long) interrupt_handler_10);
	set_kern_interrupt(11, (unsigned long) interrupt_handler_11);
	set_kern_interrupt(12, (unsigned long) interrupt_handler_12);
	set_kern_interrupt(13, (unsigned long) interrupt_handler_13);
	set_kern_interrupt(14, (unsigned long) interrupt_handler_14);
	set_kern_interrupt(16, (unsigned long) interrupt_handler_16);
	set_kern_interrupt(17, (unsigned long) interrupt_handler_17);
	set_kern_interrupt(18, (unsigned long) interrupt_handler_18);
	set_kern_interrupt(19, (unsigned long) interrupt_handler_19);
	set_kern_interrupt(20, (unsigned long) interrupt_handler_20);
	set_kern_interrupt(30, (unsigned long) interrupt_handler_30);
	/* PIC INTERRUPTS */
	set_kern_interrupt(32, (unsigned long) interrupt_handler_32);
	set_kern_interrupt(33, (unsigned long) interrupt_handler_33);
	set_kern_interrupt(34, (unsigned long) interrupt_handler_34);
	set_kern_interrupt(35, (unsigned long) interrupt_handler_35);
	set_kern_interrupt(36, (unsigned long) interrupt_handler_36);
	set_kern_interrupt(37, (unsigned long) interrupt_handler_37);
	set_kern_interrupt(38, (unsigned long) interrupt_handler_38);
	set_kern_interrupt(39, (unsigned long) interrupt_handler_39);
	set_kern_interrupt(40, (unsigned long) interrupt_handler_40);
	set_kern_interrupt(41, (unsigned long) interrupt_handler_41);
	set_kern_interrupt(42, (unsigned long) interrupt_handler_42);
	set_kern_interrupt(43, (unsigned long) interrupt_handler_43);
	set_kern_interrupt(44, (unsigned long) interrupt_handler_44);
	set_kern_interrupt(45, (unsigned long) interrupt_handler_45);
	set_kern_interrupt(46, (unsigned long) interrupt_handler_46);
	set_kern_interrupt(47, (unsigned long) interrupt_handler_47);
	set_kern_interrupt(48, (unsigned long) interrupt_handler_48);
	set_kern_interrupt(49, (unsigned long) interrupt_handler_49);
	set_kern_interrupt(50, (unsigned long) interrupt_handler_50);
	set_kern_interrupt(51, (unsigned long) interrupt_handler_51);
	set_kern_interrupt(52, (unsigned long) interrupt_handler_52);
	set_kern_interrupt(53, (unsigned long) interrupt_handler_53);
	set_kern_interrupt(54, (unsigned long) interrupt_handler_54);
	set_kern_interrupt(55, (unsigned long) interrupt_handler_55);
	set_kern_interrupt(56, (unsigned long) interrupt_handler_56);
	set_kern_interrupt(57, (unsigned long) interrupt_handler_57);
	set_kern_interrupt(58, (unsigned long) interrupt_handler_58);
	set_kern_interrupt(59, (unsigned long) interrupt_handler_59);
	set_kern_interrupt(60, (unsigned long) interrupt_handler_60);
	set_kern_interrupt(61, (unsigned long) interrupt_handler_61);
	set_kern_interrupt(62, (unsigned long) interrupt_handler_62);
	set_kern_interrupt(63, (unsigned long) interrupt_handler_63);
	set_kern_interrupt(64, (unsigned long) interrupt_handler_64);
	set_kern_interrupt(65, (unsigned long) interrupt_handler_65);
	set_kern_interrupt(66, (unsigned long) interrupt_handler_66);
	set_kern_interrupt(67, (unsigned long) interrupt_handler_67);
	set_kern_interrupt(68, (unsigned long) interrupt_handler_68);
	set_kern_interrupt(69, (unsigned long) interrupt_handler_69);
	set_kern_interrupt(70, (unsigned long) interrupt_handler_70);
	set_kern_interrupt(71, (unsigned long) interrupt_handler_71);
	set_kern_interrupt(72, (unsigned long) interrupt_handler_72);
	set_kern_interrupt(73, (unsigned long) interrupt_handler_73);
	set_kern_interrupt(74, (unsigned long) interrupt_handler_74);
	set_kern_interrupt(75, (unsigned long) interrupt_handler_75);
	set_kern_interrupt(76, (unsigned long) interrupt_handler_76);
	set_kern_interrupt(77, (unsigned long) interrupt_handler_77);
	set_kern_interrupt(78, (unsigned long) interrupt_handler_78);
	set_kern_interrupt(79, (unsigned long) interrupt_handler_79);
	set_kern_interrupt(80, (unsigned long) interrupt_handler_80);
	set_kern_interrupt(81, (unsigned long) interrupt_handler_81);
	set_kern_interrupt(82, (unsigned long) interrupt_handler_82);
	set_kern_interrupt(83, (unsigned long) interrupt_handler_83);
	set_kern_interrupt(84, (unsigned long) interrupt_handler_84);
	set_kern_interrupt(85, (unsigned long) interrupt_handler_85);
	set_kern_interrupt(86, (unsigned long) interrupt_handler_86);
	set_kern_interrupt(87, (unsigned long) interrupt_handler_87);
	set_kern_interrupt(88, (unsigned long) interrupt_handler_88);
	set_kern_interrupt(89, (unsigned long) interrupt_handler_89);
	set_kern_interrupt(90, (unsigned long) interrupt_handler_90);
	set_kern_interrupt(91, (unsigned long) interrupt_handler_91);
	set_kern_interrupt(92, (unsigned long) interrupt_handler_92);
	set_kern_interrupt(93, (unsigned long) interrupt_handler_93);
	set_kern_interrupt(94, (unsigned long) interrupt_handler_94);
	set_kern_interrupt(95, (unsigned long) interrupt_handler_95);
	set_kern_interrupt(96, (unsigned long) interrupt_handler_96);
	set_kern_interrupt(97, (unsigned long) interrupt_handler_97);
	set_kern_interrupt(98, (unsigned long) interrupt_handler_98);
	set_kern_interrupt(99, (unsigned long) interrupt_handler_99);
	set_kern_interrupt(100, (unsigned long) interrupt_handler_100);
	set_kern_interrupt(101, (unsigned long) interrupt_handler_101);
	set_kern_interrupt(102, (unsigned long) interrupt_handler_102);
	set_kern_interrupt(103, (unsigned long) interrupt_handler_103);
	set_kern_interrupt(104, (unsigned long) interrupt_handler_104);
	set_kern_interrupt(105, (unsigned long) interrupt_handler_105);
	set_kern_interrupt(106, (unsigned long) interrupt_handler_106);
	set_kern_interrupt(107, (unsigned long) interrupt_handler_107);
	set_kern_interrupt(108, (unsigned long) interrupt_handler_108);
	set_kern_interrupt(109, (unsigned long) interrupt_handler_109);
	set_kern_interrupt(110, (unsigned long) interrupt_handler_110);
	set_kern_interrupt(111, (unsigned long) interrupt_handler_111);
	set_kern_interrupt(112, (unsigned long) interrupt_handler_112);
	set_kern_interrupt(113, (unsigned long) interrupt_handler_113);
	set_kern_interrupt(114, (unsigned long) interrupt_handler_114);
	set_kern_interrupt(115, (unsigned long) interrupt_handler_115);
	set_kern_interrupt(116, (unsigned long) interrupt_handler_116);
	set_kern_interrupt(117, (unsigned long) interrupt_handler_117);
	set_kern_interrupt(118, (unsigned long) interrupt_handler_118);
	set_kern_interrupt(119, (unsigned long) interrupt_handler_119);
	set_kern_interrupt(120, (unsigned long) interrupt_handler_120);
	set_kern_interrupt(121, (unsigned long) interrupt_handler_121);
	set_kern_interrupt(122, (unsigned long) interrupt_handler_122);
	set_kern_interrupt(123, (unsigned long) interrupt_handler_123);
	set_kern_interrupt(124, (unsigned long) interrupt_handler_124);
	set_kern_interrupt(125, (unsigned long) interrupt_handler_125);
	set_kern_interrupt(126, (unsigned long) interrupt_handler_126);
	set_kern_interrupt(127, (unsigned long) interrupt_handler_127);
	set_kern_interrupt(128, (unsigned long) interrupt_handler_128);
	set_kern_interrupt(129, (unsigned long) interrupt_handler_129);
	set_kern_interrupt(130, (unsigned long) interrupt_handler_130);
	set_kern_interrupt(131, (unsigned long) interrupt_handler_131);
	set_kern_interrupt(132, (unsigned long) interrupt_handler_132);
	set_kern_interrupt(133, (unsigned long) interrupt_handler_133);
	set_kern_interrupt(134, (unsigned long) interrupt_handler_134);
	set_kern_interrupt(135, (unsigned long) interrupt_handler_135);
	set_kern_interrupt(136, (unsigned long) interrupt_handler_136);
	set_kern_interrupt(137, (unsigned long) interrupt_handler_137);
	set_kern_interrupt(138, (unsigned long) interrupt_handler_138);
	set_kern_interrupt(139, (unsigned long) interrupt_handler_139);
	set_kern_interrupt(140, (unsigned long) interrupt_handler_140);
	set_kern_interrupt(141, (unsigned long) interrupt_handler_141);
	set_kern_interrupt(142, (unsigned long) interrupt_handler_142);
	set_kern_interrupt(143, (unsigned long) interrupt_handler_143);
	set_kern_interrupt(144, (unsigned long) interrupt_handler_144);
	set_kern_interrupt(145, (unsigned long) interrupt_handler_145);
	set_kern_interrupt(146, (unsigned long) interrupt_handler_146);
	set_kern_interrupt(147, (unsigned long) interrupt_handler_147);
	set_kern_interrupt(148, (unsigned long) interrupt_handler_148);
	set_kern_interrupt(149, (unsigned long) interrupt_handler_149);
	set_kern_interrupt(150, (unsigned long) interrupt_handler_150);
	set_kern_interrupt(151, (unsigned long) interrupt_handler_151);
	set_kern_interrupt(152, (unsigned long) interrupt_handler_152);
	set_kern_interrupt(153, (unsigned long) interrupt_handler_153);
	set_kern_interrupt(154, (unsigned long) interrupt_handler_154);
	set_kern_interrupt(155, (unsigned long) interrupt_handler_155);
	set_kern_interrupt(156, (unsigned long) interrupt_handler_156);
	set_kern_interrupt(157, (unsigned long) interrupt_handler_157);
	set_kern_interrupt(158, (unsigned long) interrupt_handler_158);
	set_kern_interrupt(159, (unsigned long) interrupt_handler_159);
	set_kern_interrupt(160, (unsigned long) interrupt_handler_160);
	set_kern_interrupt(161, (unsigned long) interrupt_handler_161);
	set_kern_interrupt(162, (unsigned long) interrupt_handler_162);
	set_kern_interrupt(163, (unsigned long) interrupt_handler_163);
	set_kern_interrupt(164, (unsigned long) interrupt_handler_164);
	set_kern_interrupt(165, (unsigned long) interrupt_handler_165);
	set_kern_interrupt(166, (unsigned long) interrupt_handler_166);
	set_kern_interrupt(167, (unsigned long) interrupt_handler_167);
	set_kern_interrupt(168, (unsigned long) interrupt_handler_168);
	set_kern_interrupt(169, (unsigned long) interrupt_handler_169);
	set_kern_interrupt(170, (unsigned long) interrupt_handler_170);
	set_kern_interrupt(171, (unsigned long) interrupt_handler_171);
	set_kern_interrupt(172, (unsigned long) interrupt_handler_172);
	set_kern_interrupt(173, (unsigned long) interrupt_handler_173);
	set_kern_interrupt(174, (unsigned long) interrupt_handler_174);
	set_kern_interrupt(175, (unsigned long) interrupt_handler_175);
	set_kern_interrupt(176, (unsigned long) interrupt_handler_176);
	set_kern_interrupt(177, (unsigned long) interrupt_handler_177);
	set_kern_interrupt(178, (unsigned long) interrupt_handler_178);
	set_kern_interrupt(179, (unsigned long) interrupt_handler_179);
	set_kern_interrupt(180, (unsigned long) interrupt_handler_180);
	set_kern_interrupt(181, (unsigned long) interrupt_handler_181);
	set_kern_interrupt(182, (unsigned long) interrupt_handler_182);
	set_kern_interrupt(183, (unsigned long) interrupt_handler_183);
	set_kern_interrupt(184, (unsigned long) interrupt_handler_184);
	set_kern_interrupt(185, (unsigned long) interrupt_handler_185);
	set_kern_interrupt(186, (unsigned long) interrupt_handler_186);
	set_kern_interrupt(187, (unsigned long) interrupt_handler_187);
	set_kern_interrupt(188, (unsigned long) interrupt_handler_188);
	set_kern_interrupt(189, (unsigned long) interrupt_handler_189);
	set_kern_interrupt(190, (unsigned long) interrupt_handler_190);
	set_kern_interrupt(191, (unsigned long) interrupt_handler_191);
	set_kern_interrupt(192, (unsigned long) interrupt_handler_192);
	set_kern_interrupt(193, (unsigned long) interrupt_handler_193);
	set_kern_interrupt(194, (unsigned long) interrupt_handler_194);
	set_kern_interrupt(195, (unsigned long) interrupt_handler_195);
	set_kern_interrupt(196, (unsigned long) interrupt_handler_196);
	set_kern_interrupt(197, (unsigned long) interrupt_handler_197);
	set_kern_interrupt(198, (unsigned long) interrupt_handler_198);
	set_kern_interrupt(199, (unsigned long) interrupt_handler_199);
	set_kern_interrupt(200, (unsigned long) interrupt_handler_200);
	set_kern_interrupt(201, (unsigned long) interrupt_handler_201);
	set_kern_interrupt(202, (unsigned long) interrupt_handler_202);
	set_kern_interrupt(203, (unsigned long) interrupt_handler_203);
	set_kern_interrupt(204, (unsigned long) interrupt_handler_204);
	set_kern_interrupt(205, (unsigned long) interrupt_handler_205);
	set_kern_interrupt(206, (unsigned long) interrupt_handler_206);
	set_kern_interrupt(207, (unsigned long) interrupt_handler_207);
	set_kern_interrupt(208, (unsigned long) interrupt_handler_208);
	set_kern_interrupt(209, (unsigned long) interrupt_handler_209);
	set_kern_interrupt(210, (unsigned long) interrupt_handler_210);
	set_kern_interrupt(211, (unsigned long) interrupt_handler_211);
	set_kern_interrupt(212, (unsigned long) interrupt_handler_212);
	set_kern_interrupt(213, (unsigned long) interrupt_handler_213);
	set_kern_interrupt(214, (unsigned long) interrupt_handler_214);
	set_kern_interrupt(215, (unsigned long) interrupt_handler_215);
	set_kern_interrupt(216, (unsigned long) interrupt_handler_216);
	set_kern_interrupt(217, (unsigned long) interrupt_handler_217);
	set_kern_interrupt(218, (unsigned long) interrupt_handler_218);
	set_kern_interrupt(219, (unsigned long) interrupt_handler_219);
	set_kern_interrupt(220, (unsigned long) interrupt_handler_220);
	set_kern_interrupt(221, (unsigned long) interrupt_handler_221);
	set_kern_interrupt(222, (unsigned long) interrupt_handler_222);
	set_kern_interrupt(223, (unsigned long) interrupt_handler_223);
	set_kern_interrupt(224, (unsigned long) interrupt_handler_224);
	set_kern_interrupt(225, (unsigned long) interrupt_handler_225);
	set_kern_interrupt(226, (unsigned long) interrupt_handler_226);
	set_kern_interrupt(227, (unsigned long) interrupt_handler_227);
	set_kern_interrupt(228, (unsigned long) interrupt_handler_228);
	set_kern_interrupt(229, (unsigned long) interrupt_handler_229);
	set_kern_interrupt(230, (unsigned long) interrupt_handler_230);
	set_kern_interrupt(231, (unsigned long) interrupt_handler_231);
	set_kern_interrupt(232, (unsigned long) interrupt_handler_232);
	set_kern_interrupt(233, (unsigned long) interrupt_handler_233);
	set_kern_interrupt(234, (unsigned long) interrupt_handler_234);
	set_kern_interrupt(235, (unsigned long) interrupt_handler_235);
	set_kern_interrupt(236, (unsigned long) interrupt_handler_236);
	set_kern_interrupt(237, (unsigned long) interrupt_handler_237);
	set_kern_interrupt(238, (unsigned long) interrupt_handler_238);
	set_kern_interrupt(239, (unsigned long) interrupt_handler_239);
	set_kern_interrupt(240, (unsigned long) interrupt_handler_240);
	set_kern_interrupt(241, (unsigned long) interrupt_handler_241);
	set_kern_interrupt(242, (unsigned long) interrupt_handler_242);
	set_kern_interrupt(243, (unsigned long) interrupt_handler_243);
	set_kern_interrupt(244, (unsigned long) interrupt_handler_244);
	set_kern_interrupt(245, (unsigned long) interrupt_handler_245);
	set_kern_interrupt(246, (unsigned long) interrupt_handler_246);
	set_kern_interrupt(247, (unsigned long) interrupt_handler_247);
	set_kern_interrupt(248, (unsigned long) interrupt_handler_248);
	set_kern_interrupt(249, (unsigned long) interrupt_handler_249);
	set_kern_interrupt(250, (unsigned long) interrupt_handler_250);
	set_kern_interrupt(251, (unsigned long) interrupt_handler_251);
	set_kern_interrupt(252, (unsigned long) interrupt_handler_252);
	set_kern_interrupt(253, (unsigned long) interrupt_handler_253);
	set_kern_interrupt(254, (unsigned long) interrupt_handler_254);
	set_kern_interrupt(255, (unsigned long) interrupt_handler_255);

	/* Fill all NULL idt entries with an entry, so that we can debug it if 
	 * it was to fire (for some unknown reasons that we should have handled */
	for (int i = 0; i < 256; i++) {
		struct idt_entry temp = idt[i];
		if ( ((uint64_t)temp.offset_lower) == 0) {
			set_kern_interrupt(i, (unsigned long) interrupt_handler_1);
		}
	}

	idt_desc.limit = sizeof(idt) - 1;
	idt_desc.base = idt;
	/* Call asm funcion to load table */
	load_idt(&idt_desc);
	kprint(INFO, "IDT table loaded\n");
}