//
//  ViewController.swift
//  FaceToFace
//
//  Created by Elliott Spelman on 3/2/19.
//  Copyright © 2019 Elliott Spelman. All rights reserved.
//

import UIKit
import ARKit

class FaceToFaceViewController: UIViewController, ARSessionDelegate {
    @IBOutlet var sceneView: ARSCNView!
    @IBOutlet weak var statusLabel: UILabel!
    
    let session = ARSession()
    
    // peertalk
    let ptManager = PTManager.instance
    var peertalkConnected = false
    
    // face activity & detection
    var faceDetected: Bool!
    var faceDetectionTimer: Timer!
    var faceDetectionTimerPeriod = Double(2)
    var faceDetectionTimerIsActive = false
    
    // face data
    var _tmpFaceDataDict: [String: Float] = [:]
    var _tmpFaceDataNSDict: NSDictionary = [:]
    var _tmpPRSDict: [String: Float] = [:]
    var _tmpPRSNSDict: NSDictionary = [:]
    var _tmpPositionWorld = SCNVector3()
    var _tmpPositionScreen = SCNVector3()
    var _tmpQuaternion = SCNQuaternion()
    var _tmpScale = SCNVector3()
    var _tmpExpressions: [String : Float] = [:]
    var _tmpExpressionNSDict: NSDictionary = [:]
    
    // face data timers
    var faceDataTimer = Timer()
    var faceDataTimerPeriod = Double(0.1)
    var faceDataTimerIsActive = false
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        guard ARFaceTrackingConfiguration.isSupported else {
            fatalError("Face tracking is not supported on this device")
        }
        
        // Set up the scene view
        self.sceneView.backgroundColor = .clear
        self.sceneView.scene = SCNScene()
        self.sceneView.rendersContinuously = true
        
        // Set up the AR configuration
        let config = ARFaceTrackingConfiguration()
        config.worldAlignment = .gravity
        
        // change to SCNView instead of ARSCNView if you don't want to render the AR Stuff
        // session.delegate = self
        // sceneViewsession.run(config, options: [])
        // self.sceneView.delegate = self
        sceneView.session.run(config, options: [])
        sceneView.session.delegate = self
        sceneView.delegate = self
        
        // Set up peertalk
        ptManager.delegate = self
        ptManager.connect(portNumber: PORT_NUMBER)
        
        self._tmpPositionWorld = SCNVector3(0, 0, 0)
        self._tmpPositionScreen = SCNVector3(0, 0, 0)
        self._tmpQuaternion = SCNQuaternion(0, 0, 0, 0)
        self._tmpScale = SCNVector3(1, 1, 1)
        
        self.faceDetected = false
    }
    
    override func viewWillAppear(_ animated: Bool) {
        super.viewWillAppear(animated)
        if (!peertalkConnected) {
            ptManager.connect(portNumber: PORT_NUMBER)
        }
    }
    
    override func viewWillDisappear(_ animated: Bool) {
        super.viewWillDisappear(animated)
        session.pause()
    }
    
    
    // MARK: - User Actions
    
    
    @IBAction func startGame(_ sender: Any) {
        self.startGame()
    }
    
    func startGame() {
        // do stuff
    }
    
    
    // MARK: - AR Session
    
    
    var currentFaceAnchor: ARFaceAnchor?
    var currentFrame: ARFrame?
    
    func session(_ session: ARSession, didUpdate frame: ARFrame) {
        self.currentFrame = frame
    }
    
    func session(_ session: ARSession, didAdd anchors: [ARAnchor]) {
    }
    
    func session(_ session: ARSession, didUpdate anchors: [ARAnchor]) {
        guard let faceAnchor = anchors.first as? ARFaceAnchor else { return }
        self.currentFaceAnchor = faceAnchor
        
        DispatchQueue.main.async {
            // need to call heart beat on main thread
            self.processARKitFaceData()
            
            if (self.faceDetected == false) {
                self.sendNewFaceDetected()
                self.faceDetected = true
            }
            
            if (self.faceDetectionTimer != nil) {
                self.faceDetectionTimer.invalidate()
            }
            self.faceDetectionTimer = Timer.scheduledTimer(timeInterval: 2, target: self, selector: (#selector(self.faceDetectionTimerExpired(_:))), userInfo: nil, repeats: false)
        }
    }
    
    func session(_ session: ARSession, didRemove anchors: [ARAnchor]) {
//        sendFaceHasLeft()
    }
    
    func processARKitFaceData() {
        
        if ((peertalkConnected) && (faceDataTimerIsActive == false)) {
            
            // convert Swift face data dictionaries to NSDictionaries and pass to peertalk
            self._tmpPRSNSDict = self._tmpPRSDict as NSDictionary
            self._tmpExpressionNSDict = self._tmpExpressions as NSDictionary
            
            self.ptManager.sendDictionary(dictionary: self._tmpPRSNSDict, type: PTType.PTFacePRSData.rawValue)
            self.ptManager.sendDictionary(dictionary: self._tmpExpressionNSDict, type: PTType.PTFaceExpressionData.rawValue)
            
            startFaceDataTimer()
        }
    }
    
    func sendNewFaceDetected() {
        if (ptManager.isConnected) {
            let num = 10
            self.ptManager.sendObject(object: num, type: PTType.PTFaceHasArrived.rawValue)
            print("sent faceHasArrived message - ")
            print(Date())
        } else {
            print("attempted to send new face detected message, but ptManager is not connected")
        }
    }
    
    func sendFaceHasLeft() {
        if (ptManager.isConnected) {
            let num = 10
            self.ptManager.sendObject(object: num, type: PTType.PTFaceHasLeft.rawValue)
            print("sent faceHasLeft message")
            print(Date())
        } else {
            print("attempted to send face has left message, but ptManager is not connected")
        }
    }
    
    // TIMERS
    
    func startFaceDataTimer() {
        faceDataTimer = Timer.scheduledTimer(timeInterval: faceDataTimerPeriod, target: self, selector: (#selector(denoiseTimerExpired)), userInfo: nil, repeats: false)
        faceDataTimerIsActive = true
    }
    
    @objc func denoiseTimerExpired() {
        faceDataTimerIsActive = false
    }
    
    @objc func faceDetectionTimerExpired(_ sender:Timer) {
        self.sendFaceHasLeft()
        self.faceDetected = false
    }
    
}

extension FaceToFaceViewController: PTManagerDelegate {
    
    func peertalk(shouldAcceptDataOfType type: UInt32) -> Bool {
        return true
    }
    
    func peertalk(didReceiveData data: Data, ofType type: UInt32, withTag tag: UInt32) {
        if type == PTType.PTExampleFrameTypeDeviceInfo.rawValue {
            //let count = data.convert() as! Int
            //self.label.text = "\(count)"
        } else if type == PTType.PTExampleFrameTypeTextMessage.rawValue {
            //let image = UIImage(data: data)
            //self.imageView.image = image
        } else if type == PTType.PTExampleFrameTypePing.rawValue {
            print("got a ping")
            let pongMessage = "send a pong"
            self.ptManager.sendPong(object: pongMessage, type: PTType.PTExampleFrameTypePong.rawValue, tagno: tag, completion: nil)
        } else if type == PTType.PTExampleFrameTypePong.rawValue {
            
        }
    }
    
    func peertalk(didChangeConnection connected: Bool) {
        print("Connection: \(connected)")
        peertalkConnected = connected
        self.statusLabel.text = connected ? "Connected" : "Disconnected"
    }
    
}

extension FaceToFaceViewController: ARSCNViewDelegate {
    func renderer(_ renderer: SCNSceneRenderer, nodeFor anchor: ARAnchor) -> SCNNode? {
        
        guard let device = sceneView.device else {
            return nil
        }
        let faceGeometry = ARSCNFaceGeometry(device: device)
        let node = SCNNode(geometry: faceGeometry)
        node.geometry?.firstMaterial?.fillMode = .lines
        
        return node
    }
    
    func renderer(_ renderer: SCNSceneRenderer, didUpdate node: SCNNode, for anchor: ARAnchor) {
        
        guard let faceAnchor = anchor as? ARFaceAnchor,
            let faceGeometry = node.geometry as? ARSCNFaceGeometry else {
                return
        }
        
        self._tmpPositionWorld = node.position
        self._tmpQuaternion = node.orientation
        self._tmpScale = node.scale
        self._tmpPositionScreen = renderer.projectPoint(self._tmpPositionWorld)
        
        self._tmpPRSDict = [
            "pX" : self._tmpPositionWorld.x,
            "pY" : self._tmpPositionWorld.y,
            "pZ" : self._tmpPositionWorld.z,
            "pXs" : self._tmpPositionScreen.x,
            "pYs" : self._tmpPositionScreen.y,
            "pZs" : self._tmpPositionScreen.z,
            "rX" : self._tmpQuaternion.x,
            "rY" : self._tmpQuaternion.y,
            "rZ" : self._tmpQuaternion.z,
            "rW" : self._tmpQuaternion.w,
            "sX" : self._tmpScale.x,
            "sY" : self._tmpScale.y,
            "sZ" : self._tmpScale.z,
        ]
        
        for (location, value) in faceAnchor.blendShapes {
            let locationString: String = location.rawValue
            let valueFloat: Float = value.floatValue
            self._tmpExpressions[locationString] = valueFloat
        }

        faceGeometry.update(from: faceAnchor.geometry)
    }
}

extension matrix_float4x4 {
    func position() -> SCNVector3 {
        return SCNVector3(columns.3.x, columns.3.y, columns.3.z)
    }
}

extension Dictionary {
    mutating func merge(dict: [Key: Value]){
        for (k, v) in dict {
            updateValue(v, forKey: k)
        }
    }
}

