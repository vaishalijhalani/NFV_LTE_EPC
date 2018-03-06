#ifndef VARIABLE_H
#define VARIABLE_H

/* EMM state 
	 * 0 - Deregistered
	 * 1 - Registered 
	 */	
	int emm_state; /* EPS Mobililty Management state */

	/* ECM state 
	 * 0 - Disconnected
	 * 1 - Connected 
	 * 2 - Idle 
	 */
	int ecm_state; /* EPS Connection Management state */

	/* UE id */
	uint64_t imsi; /* International Mobile Subscriber Identity */
	uint64_t guti; /* Globally Unique Temporary Identifier */
	string ip_addr;
	uint32_t enodeb_s1ap_ue_id; /* eNodeB S1AP UE ID */
	uint32_t mme_s1ap_ue_id; /* MME S1AP UE ID */

	/* UE location info */
	uint64_t tai; /* Tracking Area Identifier */
	vector<uint64_t> tai_list; /* Tracking Area Identifier list */
	uint64_t tau_timer;

	/* UE security context */
	uint64_t key; /* Primary key used in generating secondary keys */
	uint64_t k_asme; /* Key for Access Security Management Entity */
	uint64_t ksi_asme; /* Key Selection Identifier for Access Security Management Entity */
	uint64_t k_nas_enc; /* Key for NAS Encryption / Decryption */
	uint64_t k_nas_int; /* Key for NAS Integrity check */
	uint64_t nas_enc_algo; /* Idenitifier of NAS Encryption / Decryption */
	uint64_t nas_int_algo; /* Idenitifier of NAS Integrity check */
	uint64_t count;
	uint64_t bearer;
	uint64_t dir;

	/* EPS info, EPS bearer info */
	uint64_t apn_in_use; /* Access Point Name in Use */
	uint8_t eps_bearer_id; /* Evolved Packet System Bearer ID */
	uint8_t e_rab_id; /* Evolved Radio Access Bearer ID */
	uint32_t s1_uteid_ul; /* S1 Userplane Tunnel Endpoint Identifier - Uplink */
	uint32_t s1_uteid_dl; /* S1 Userplane Tunnel Endpoint Identifier - Downlink */

	/* Network Operator info */
	uint16_t mcc; /* Mobile Country Code */
	uint16_t mnc; /* Mobile Network Code */
	uint16_t plmn_id; /* Public Land Mobile Network ID */	
	uint64_t msisdn; /* Mobile Station International Subscriber Directory Number - Mobile number */
	uint16_t nw_capability;

	/* Parameters added for hand-over  */
	uint16_t eNodeB_id; /* EnodeB identifier */ // we could send this at all msg
	uint16_t handover_target_eNodeB_id; /* EnodeB identifier */

	bool inHandover; /* handover phase*/

	//when inHandover = true use the below uplink
	uint32_t indirect_s1_uteid_ul;
	// handover changes end/

	void rancontext();
	void initvariable();


	#endif